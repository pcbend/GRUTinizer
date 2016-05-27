#!/usr/bin/env python

import numpy as np
from ROOT import TVector3
from ROOT import TRotation

class CloverType:
    Yale, Tohoku, IMP = range(3)

class clover(object):
    def __init__(self,r,theta,phi,clovertype=CloverType.Yale):
        self.xdispl = 0
        self.ydispl = 0
        self.placement = TVector3(1.,1.,1.)
        self.placement.SetMag(r)
        self.placement.SetTheta(theta*np.pi/180)
        self.placement.SetPhi(phi*np.pi/180)
        self.type = clovertype

        if clovertype == CloverType.Yale:
            self.xdispl = self.ydispl = 2.23
        if clovertype == CloverType.Tohoku:
            self.xdispl = self.ydispl = 2.23
        if clovertype == CloverType.IMP:
            self.xdispl = self.ydispl = 2.23

    def print_clover_placement(self,idxclover):
        print("clover.{0:02d}.vec: ".format(idxclover) + str(np.around(self.placement.X(),10)) + " " + str(np.around(self.placement.Y(),10)) + " " + str(np.around(self.placement.Z(),10)))
    def print_clover_placement_sph(self,idxclover):
        print("clover.{0:02d}.vec_sph: ".format(idxclover) + str(np.around(self.placement.Mag(),10)) + " " + str(np.around(self.placement.Theta()*180/np.pi,10)) + " " + str(np.around(self.placement.Phi()*180/np.pi,10)))
    def print_crystal_placement(self,idxclover):
        for k,(xsign,ysign) in enumerate([(-1,+1),(+1,+1),(-1,-1),(+1,-1)]):
            theta_rotation = TRotation()
            phi_rotation = TRotation()
            theta_rotation.RotateY(self.placement.Theta())
            phi_rotation.RotateZ(self.placement.Phi())
            crystal_offset = TVector3(xsign*self.xdispl,ysign*self.ydispl,0)
            crystal_offset = phi_rotation*theta_rotation*crystal_offset
            crystal = TVector3(self.placement.X()+crystal_offset.X(),self.placement.Y()+crystal_offset.Y(),self.placement.Z()+crystal_offset.Z())
            print("clover.{0:02d}.{1}.vec: ".format(idxclover,k+1) + str(np.around(crystal.X(),10)) + " " + str(np.around(crystal.Y(),10)) + " " + str(np.around(crystal.Z(),10)))
        print("")

    def print_crystal_placement_sph(self,idxclover):
        for k,(xsign,ysign) in enumerate([(-1,+1),(+1,+1),(-1,-1),(+1,-1)]):
            theta_rotation = TRotation()
            phi_rotation = TRotation()
            theta_rotation.RotateY(self.placement.Theta())
            phi_rotation.RotateZ(self.placement.Phi())
            crystal_offset = TVector3(xsign*self.xdispl,ysign*self.ydispl,0)
            crystal_offset = phi_rotation*theta_rotation*crystal_offset
            crystal = TVector3(self.placement.X()+crystal_offset.X(),self.placement.Y()+crystal_offset.Y(),self.placement.Z()+crystal_offset.Z())
            print("clover.{0:02d}.{1}.vec_sph: ".format(idxclover,k+1) + str(np.around(crystal.Mag(),10)) + " " + str(np.around(crystal.Theta()*180/np.pi,10)) + " " + str(np.around(crystal.Phi()*180/np.pi,10)))
        print("")


    def set_placement(self, _placement):
        self.placement = _placement

if __name__=='__main__':

    # 45 degree detectors (Tohoku)
    for i in range(0,4):
        r = 17.70
        theta = 45.0
        phi = 45+90.0*i
        if phi > 180:
            phi -= 360
        placed = clover(r,theta,phi,CloverType.Yale)
        #placed.print_clover_placement_sph(i+1)
        #placed.print_clover_placement(i+1)
        placed.print_crystal_placement(i+1)
        #placed.print_crystal_placement_sph(i+1)


    # 90 degree detectors (yale)
    for i in range(0,8):
        r = 17.70
        theta = 90.0
        phi = 45*i
        placed = clover(r,theta,phi,CloverType.Yale)
        #placed.print_clover_placement_sph(i+4+1)
        #placed.print_clover_placement(i+4+1)
        placed.print_crystal_placement(i+4+1)
        #placed.print_crystal_placement_sph(i+4+1)

    # 135 degree detectors (Tohoku)
    for i in range(0,4):
        r = 17.70
        theta = 135.0
        phi = 45+90.0*i
        if phi > 180:
            phi -= 360
        placed = clover(r,theta,phi,CloverType.Yale)
        #placed.print_clover_placement_sph(i+12+1)
        #placed.print_clover_placement(i+12+1)
        placed.print_crystal_placement(i+12+1)
        #placed.print_crystal_placement_sph(i+12+1)
