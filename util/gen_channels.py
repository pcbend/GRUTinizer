#!/usr/bin/env python3

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
        system=2, crate=0, slot=slot, channum=channum)

def janus_name(detnum, frontbacktime, channum):
    return 'JAN{detnum:02d}{FBT}{NP}{channum:02}'.format(
        detnum=detnum, FBT=frontbacktime, NP='N', channum=channum)


f = open('channels.cal','w')

for channum in range(16):
    address = sega_address(1, 6, channum)
    name = sega_name(channum+1, 0)
    f.write(channel_format.format(name=name, address=address))

for slotnum in range(3,9):
    for channum in range(16):
        address = sega_address(3, slotnum, channum)
        detnum = {3:1,
                  4:1,
                  5:3,
                  6:3,
                  7:5,
                  8:5}[slotnum]
        name = sega_name(detnum, (0 if slotnum%2==1 else 16) + channum + 1)
        f.write(channel_format.format(name=name, address=address))

for slotnum in range(5, 9):
    for channum in range(32):
        address = janus_address(slotnum, channum)
        detnum = 0 if slotnum < 7 else 1
        frontbacktime = 'F' if (slotnum==5 or slotnum==7) else 'B'
        name = janus_name(detnum, frontbacktime, channum)
        f.write(channel_format.format(name=name, address=address))


# for slotnum in range(9, 13):
#     for channum in range(32):
#         address = janus_address(slotnum, channum)
#         detnum = 0 if slotnum < 9 else 1
#         frontbacktime = 'T'
#         name = janus_name(detnum, frontbacktime, channum)
#         f.write(channel_format.format(name=name, address=address))

f.close()
