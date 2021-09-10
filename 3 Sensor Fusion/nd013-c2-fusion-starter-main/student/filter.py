# ---------------------------------------------------------------------
# Project "Track 3D-Objects Over Time"
# Copyright (C) 2020, Dr. Antje Muntzinger / Dr. Andreas Haja.
#
# Purpose of this file : Kalman filter class
#
# You should have received a copy of the Udacity license together with this program.
#
# https://www.udacity.com/course/self-driving-car-engineer-nanodegree--nd013
# ----------------------------------------------------------------------
#

# imports
import numpy as np

# add project directory to python path to enable relative imports
import os
import sys

import misc.params

PACKAGE_PARENT = '..'
SCRIPT_DIR = os.path.dirname(os.path.realpath(os.path.join(os.getcwd(), os.path.expanduser(__file__))))
sys.path.append(os.path.normpath(os.path.join(SCRIPT_DIR, PACKAGE_PARENT)))
import misc.params as params


def get_F(dt, n):
    F = np.eye(2 * n)
    for id in range(n):
        F[id, id + n] = dt
    return F

def get_Q(dt, n, sigma):
    dt2 = dt * dt
    dt3 = dt2 * dt
    dt4 = dt3 * dt
    Q = np.diagflat([[dt4 / 4] * n, [dt2] * n])
    Qside = np.eye(2 * n, k=n) * dt3 / 2
    return (Q + Qside) * sigma


class Filter:
    '''Kalman filter class'''
    def __init__(self):
        dt = params.dt
        n = params.dim_state // 2
        self.__F = get_F(dt, n)
        self.__Q = get_Q(dt, n, params.q)
        pass

    def F(self):
        return self.__F

        
        ############
        # END student code
        ############ 

    def Q(self):
        return self.__Q
        
        ############
        # END student code
        ############ 

    def predict(self, track):
        F = self.F()
        Q = self.Q()
        x = F * track.x
        P = F * track.P * F.T + Q
        track.set_x(x)
        track.set_P(P)
        pass
        
        ############
        # END student code
        ############ 

    def update(self, track, meas):
        x = track.x
        H = meas.sensor.get_H(x)
        S = self.S(track, meas, H)
        K = track.P * H.T * S.I
        gamma = self.gamma(track, meas)

        x = track.x + K * gamma  # state update
        I = np.eye(params.dim_state)
        P = (I - K * H) * track.P  # covariance update
        track.set_x(x)
        track.set_P(P)
        track.update_attributes(meas)

        ############
        # END student code
        ############ 
        track.update_attributes(meas)
    
    def gamma(self, track, meas):
        return meas.z - meas.sensor.get_hx(track.x)
        
        ############
        # END student code
        ############ 

    def S(self, track, meas, H):
        return H * track.P * H.transpose() + meas.R
        
        ############
        # END student code
        ############ 