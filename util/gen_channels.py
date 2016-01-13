#!/usr/bin/env python3

import sys


sega = 1
janus = 2

channel_format = """
{name} {{
  Address: {address}
  EnergyCoeff: 0 1
}}
"""

def sega_address(crate, slot, channel):
    return '0x{system:02x}{crate:02x}{slot:02x}{channel:02x}'.format(
        system=1,crate=crate, slot=slot, channel=channel)

def sega_name(detnum, segnum):
    return 'SEG{detnum:02d}A{NP}{segnum:02d}'.format(
        detnum=detnum, NP=('N' if segnum!=0 else 'P'), segnum=segnum)

def janus_address(slot, channum):
    return '0x{system:02x}{crate:02x}{slot:02x}{channum:02x}'.format(
        system=2, crate=4, slot=slot, channum=channum)

def janus_name(detnum, frontbacktime, segmentnum):
    system = 'JAN' if segmentnum>0 else 'DEL'
    return '{system}{detnum:02d}{FBT}{NP}{segmentnum:02}'.format(
        detnum=detnum, FBT=frontbacktime, NP='N', segmentnum=segmentnum,
        system=system)


f = open(sys.argv[1],'w')

for channum in range(16):
    address = sega_address(1, 2, channum)
    name = sega_name(channum+1, 0)
    f.write(channel_format.format(name=name, address=address))

detnums = {(1,3):1,
           (1,4):1,
           (1,5):2,
           (1,6):2,
           (1,7):3,
           (1,8):3,
           (1,9):4,
           (1,10):4,

           (2,2):5,
           (2,3):5,
           (2,4):6,
           (2,5):6,
           (2,6):7,
           (2,7):7,
           (2,8):8,
           (2,9):8,
           (2,10):9,
           (2,11):9,
           (2,12):10,
           (2,13):10,

           (3,2):11,
           (3,3):11,
           (3,4):12,
           (3,5):12,
           (3,6):13,
           (3,7):13,
           (3,8):14,
           (3,9):14,
           (3,10):15,
           (3,11):15,
           (3,12):16,
           (3,13):16,
           }

for cratenum in range(1,4):
    for slotnum in range(2,14):
        for channum in range(16):
            address = sega_address(cratenum, slotnum, channum)
            try:
                detnum = detnums[(cratenum,slotnum)]
            except KeyError:
                continue

            if cratenum==1:
                # In crate 1, A1 (channum==1) is on slots 3,5,...
                channum = (0 if slotnum%2==1 else 16) + channum + 1
            else:
                # In crate 2 and 3, A1 (channum==1) is in slots 2,4,...
                channum = (0 if slotnum%2==0 else 16) + channum + 1

            name = sega_name(detnum, channum)
            f.write(channel_format.format(name=name, address=address))

# map from channum (0-127) to (detnum, segmentnum)
janus_mapping = { }
for ringnum in range(1,25):
    # Upstream rings 1-24 go to channels 23-0
    janus_mapping[24-ringnum] = (0,ringnum)
    # Downstream rings 1-24 go to channels 87-64
    janus_mapping[88-ringnum] = (1,ringnum)

for sectornum in range(10,33):
    # Upstream sectors 10-32 go to channels 32-54
    janus_mapping[32 + (sectornum-10)] = (0, sectornum)
    # Downstream sectors 10-32 go to channels 96-118
    janus_mapping[96 + (sectornum-10)] = (1, sectornum)

for sectornum in range(1, 10):
    # Upstream sectors 1-9 go to channels 55-63
    janus_mapping[55 + (sectornum-1)] = (0, sectornum)
    # Downstream sectors 1-9 go to channels 119-123
    janus_mapping[119 + (sectornum-1)] = (1, sectornum)

for channum in range(24,32):
    janus_mapping[channum] = (0,0)
for channum in range(88,96):
    janus_mapping[channum] = (0,0)

# Make the TChannel format for JANUS
for slotnum in range(5, 9):
    for channum in range(32):
        address = janus_address(slotnum, channum)
        detnum = 0 if slotnum < 7 else 1
        frontbacktime = 'F' if (slotnum==5 or slotnum==7) else 'B'

        shaper_channum = 32*(slotnum-5) + channum
        _,segmentnum = janus_mapping[shaper_channum]

        name = janus_name(detnum, frontbacktime, segmentnum)
        f.write(channel_format.format(name=name, address=address))

f.close()
