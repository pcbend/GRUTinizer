#!/usr/bin/env python

import numpy

def rotation_x(theta, measure='radians'):
    if measure == 'degrees':
        theta *= numpy.pi/180.0
    costheta = numpy.cos(theta)
    sintheta = numpy.sin(theta)

    return numpy.matrix([[1,        0,         0, 0],
                         [0, costheta, -sintheta, 0],
                         [0, sintheta,  costheta, 0],
                         [0,        0,         0, 1]])

def rotation_y(theta, measure='radians'):
    if measure == 'degrees':
        theta *= numpy.pi/180.0
    costheta = numpy.cos(theta)
    sintheta = numpy.sin(theta)

    return numpy.matrix([[costheta, 0, -sintheta, 0],
                         [       0, 1,         0, 0],
                         [sintheta, 0,  costheta, 0],
                         [       0, 0,         0, 1]])

def rotation_z(theta, measure='radians'):
    if measure == 'degrees':
        theta *= numpy.pi/180.0
    costheta = numpy.cos(theta)
    sintheta = numpy.sin(theta)

    return numpy.matrix([[costheta, -sintheta, 0, 0],
                         [sintheta,  costheta, 0, 0],
                         [       0,         0, 1, 0],
                         [       0,         0, 0, 1]])

def translation(dx, dy, dz):
    return numpy.matrix([[1, 0, 0, dx],
                         [0, 1, 0, dy],
                         [0, 0, 1, dz],
                         [0, 0, 0, 1]])


def scale(sx, sy, sz):
    return numpy.matrix([[sx, 0,  0,  0],
                         [0,  sy, 0,  0],
                         [0,  0,  sz, 0],
                         [0,  0,  0,  1]])

def identity():
    return numpy.matrix(numpy.identity(4))
