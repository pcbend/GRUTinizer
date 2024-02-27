#!/usr/bin/env python

import numpy

from homogeneous import rotation_y, rotation_z, translation

def generate_matrices():
    displacement = 13.25 # Distance from z-axis of beampipe to z-axis of crystals
    z_shift = -1.2 # Distance from the xy-plane to the front face of the crystal.
    initial_angle = 90 + 22.5 # phi angle of frame position 1, relative to the x axis.
    delta_angle = 45 # phi angle difference between each detector

    # Generate each position
    dets = {}
    for framepos in range(1,9):
        phi_angle = initial_angle + (framepos-1)*delta_angle
        arr = (translation(0, 0, z_shift) *
               rotation_z(phi_angle, 'degrees') *
               translation(displacement, 0, 0) *
               rotation_z(90, 'degrees') *
               rotation_y(180, 'degrees')
               )
        dets[framepos] = arr

        # framepos+8 is the rotation of framepos about the y axis by 180 degrees.
        dets[framepos+8] = rotation_y(180, 'degrees') * arr

    #Clean up the result for printing
    #Rounding errors lead to some values on the order or 1e-16, which I don't care about.
    for framepos in dets:
        dets[framepos] = numpy.matrix(numpy.around(dets[framepos]+1e-12, 10))

    return dets


def segment_pos(segnum):
    segment_phi = numpy.pi/4.0
    segment_z = 0.5
    perp_distance = 1.485

    crystal_phi = segment_phi + (segnum - 1) * (numpy.pi/2.0)
    crystal_z = segment_z + (segnum/4)

    return numpy.matrix([[perp_distance*numpy.cos(crystal_phi)],
                         [perp_distance*numpy.sin(crystal_phi)],
                         [crystal_z],
                         [1]])


def save_matrices_root(dets, filename):
    with open(filename,'w') as f:
        for framepos, arr in dets.items():
            f.write('# Frame position {}\n'.format(framepos))
            f.write('det.{:02d}.origin: {} {} {}\n'.format(framepos-1, arr[0,3], arr[1,3], arr[2,3]))
            f.write('det.{:02d}.x_vect: {} {} {}\n'.format(framepos-1, arr[0,0], arr[1,0], arr[2,0]))
            f.write('det.{:02d}.y_vect: {} {} {}\n'.format(framepos-1, arr[0,1], arr[1,1], arr[2,1]))
            f.write('det.{:02d}.z_vect: {} {} {}\n'.format(framepos-1, arr[0,2], arr[1,2], arr[2,2]))
            f.write('\n')

def save_matrices_spectcl(dets, filename):
    with open(filename, 'w') as f:
        for framepos, arr in dets.items():
            f.write('# Frame position {}\n'.format(framepos))
            for segnum in range(32):
                vec = arr * segment_pos(segnum)
                x = vec[0,0]
                y = vec[1,0]
                z = vec[2,0]
                f.write('set sega.dets{detnum:02d}.seg.x.{segnum:02d} {x}\n'.format(
                        detnum=framepos-1,segnum=segnum,x=x,y=y,z=z))
                f.write('set sega.dets{detnum:02d}.seg.y.{segnum:02d} {y}\n'.format(
                        detnum=framepos-1,segnum=segnum,x=x,y=y,z=z))
                f.write('set sega.dets{detnum:02d}.seg.z.{segnum:02d} {z}\n'.format(
                        detnum=framepos-1,segnum=segnum,x=x,y=y,z=z))
            f.write('\n')


def plot_matrices(dets):
    import pylab
    fig = pylab.figure(figsize=(7,10))
    axes_upstream = fig.add_subplot(211)
    axes_downstream = fig.add_subplot(212)

    for framepos, arr in dets.items():
        axes = axes_upstream if arr[2,3] < 0 else axes_downstream
        axes.arrow(arr[0,3], arr[1,3], # Origin
                   arr[0,0], arr[1,0], # x direction
                   color='red')
        axes.arrow(arr[0,3], arr[1,3], # Origin
                   arr[0,1], arr[1,1], # x direction
                   color='green')


    axes_upstream.set_xlim(-15,15)
    axes_upstream.set_ylim(-15,15)
    axes_downstream.set_xlim(-15,15)
    axes_downstream.set_ylim(-15,15)

    axes_upstream.set_title('Upstream\n'
                            'Red = crystal x-axis (should go clockwise)\n'
                            'Green = crystal y-axis (should be radially inward)')
    axes_downstream.set_title('Downstream\n'
                              'Red = crystal x-axis (should go counterclockwise)\n'
                              'Green = crystal y-axis (should be radially inward)')

    fig.tight_layout()
    pylab.show()


if __name__=='__main__':
    import sys
    dets = generate_matrices()
    #plot_matrices(dets)
    save_matrices_root(dets, sys.argv[1])
    #save_matrices_spectcl(dets, sys.argv[1])
