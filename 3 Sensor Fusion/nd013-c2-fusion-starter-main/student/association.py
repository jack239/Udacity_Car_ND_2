# ---------------------------------------------------------------------
# Project "Track 3D-Objects Over Time"
# Copyright (C) 2020, Dr. Antje Muntzinger / Dr. Andreas Haja.
#
# Purpose of this file : Data association class with single nearest neighbor association and gating based on Mahalanobis distance
#
# You should have received a copy of the Udacity license together with this program.
#
# https://www.udacity.com/course/self-driving-car-engineer-nanodegree--nd013
# ----------------------------------------------------------------------
#

# imports
import numpy as np
from scipy.stats.distributions import chi2

# add project directory to python path to enable relative imports
import os
import sys
PACKAGE_PARENT = '..'
SCRIPT_DIR = os.path.dirname(os.path.realpath(os.path.join(os.getcwd(), os.path.expanduser(__file__))))
sys.path.append(os.path.normpath(os.path.join(SCRIPT_DIR, PACKAGE_PARENT)))

import misc.params as params 

class Association:
    '''Data association class with single nearest neighbor association and gating based on Mahalanobis distance'''
    def __init__(self):
        self.association_matrix = np.matrix([])
        self.unassigned_tracks = []
        self.unassigned_meas = []
        
    def associate(self, track_list, meas_list, KF):
             
        ############
        # TODO Step 3: association:
        # - replace association_matrix with the actual association matrix based on Mahalanobis distance (see below) for all tracks and all measurements
        # - update list of unassigned measurements and unassigned tracks
        ############
        
        # the following only works for at most one track and one measurement
        N = len(track_list) # N tracks
        M = len(meas_list) # M measurements

        self.association_matrix = np.inf*np.ones((N, M))
        for track_id in range(N):
            track = track_list[track_id]
            for meas_id in range(M):
                meas = meas_list[meas_id]
                dist = self.MHD(track, meas, KF)
                if self.gating(dist, meas.sensor):
                    self.association_matrix[track_id, meas_id] = dist


        self.unassigned_tracks = list(range(N)) # reset lists
        self.unassigned_meas = list(range(M))

        ############
        # END student code
        ############ 
                
    def get_closest_track_and_meas(self):
        if len(self.unassigned_meas) == 0 or len(self.unassigned_tracks) == 0:
            return np.nan, np.nan
        pre_result = np.unravel_index(np.argmin(self.association_matrix, axis=None), self.association_matrix.shape)
        if np.isinf(self.association_matrix[pre_result[0], pre_result[1]]):
            return np.nan, np.nan
        result = self.unassigned_tracks[pre_result[0]], self.unassigned_meas[pre_result[1]]
        del self.unassigned_tracks[pre_result[0]],
        del self.unassigned_meas[pre_result[1]]
        self.association_matrix = np.delete(np.delete(self.association_matrix, pre_result[0], axis=0), pre_result[1], axis=1)
        return result

    def gating(self, MHD, sensor):
        return MHD < chi2.ppf(params.gating_threshold, 2)
        
    def MHD(self, track, meas, KF):

        # calc Mahalanobis distance
        gamma = meas.z - meas.sensor.get_hx(track.x)
        H = meas.sensor.get_H(track.x)
        S = H * track.P * H.T + meas.R
        MHD = gamma.T * S.I * gamma # Mahalanobis distance formula
        return MHD

        pass
        
        ############
        # END student code
        ############ 
    
    def associate_and_update(self, manager, meas_list, KF):
        # associate measurements and tracks
        self.associate(manager.track_list, meas_list, KF)
    
        # update associated tracks with measurements
        while self.association_matrix.shape[0]>0 and self.association_matrix.shape[1]>0:
            
            # search for next association between a track and a measurement
            ind_track, ind_meas = self.get_closest_track_and_meas()
            if np.isnan(ind_track):
                print('---no more associations---')
                break
            track = manager.track_list[ind_track]
            
            # check visibility, only update tracks in fov    
            if not meas_list[0].sensor.in_fov(track.x):
                continue
            
            # Kalman update
            print('update track', track.id, 'with', meas_list[ind_meas].sensor.name, 'measurement', ind_meas)
            KF.update(track, meas_list[ind_meas])
            
            # update score and track state 
            manager.handle_updated_track(track)
            
            # save updated track
            manager.track_list[ind_track] = track
            
        # run track management 
        manager.manage_tracks(self.unassigned_tracks, self.unassigned_meas, meas_list)
        
        for track in manager.track_list:            
            print('track', track.id, 'score =', track.score)