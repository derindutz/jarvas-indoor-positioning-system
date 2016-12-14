%% CS 241 Range-based positioning test

clear all; close all;

% *** HOW TO USE THIS ***
% Update parameters in the "User input" section for whatever you'd like to
% test
% *** RESULTS ***
% The program will give you several outputs:
% ranges_measured - The ranges (including added noise) that were fed into
%     the estimation
% estimate - The estimated position of the free node
% error - The distance between the estimated position of the free node and
%     the actual position
% Plot - The plot shows the locations of the beacons (in red), the
%     estimated position (in yellow), and the actual position (in magenta)

%% User input

% Known locations of beacons
beacons = [1   8;
           10  3;
           2   30];

% Where is the user (this script automatically calculates ranges and adds
% noise to them)
actual_pos = [12 10];

% Maximum measurement error
meas_err_max = 2;

%% Generate test data

ranges = [pdist([beacons(1,:); actual_pos], 'euc');
               pdist([beacons(2,:); actual_pos], 'euc');
               pdist([beacons(3,:); actual_pos], 'euc')];

ranges_measured = ranges + meas_err_max*rand([3 1])

%% Position estimation
% Based on Equation 10 from this paper:
% Linear least squares localization in sensor networks
% DOI: 10.1186/s13638-015-0298-1
% http://jwcn.eurasipjournals.springeropen.com/articles/10.1186/s13638-015-0298-1

% These calculations only need to be done once (after the beacon locations
% are known)
A = [(-2 * beacons) ones(3,1)];
pinvA = pinv(A);

% These calculations must be performed every time a new position is
% estimated
y = ranges_measured.^2 - beacons(:,1).^2 - beacons(:,2).^2;
x = pinvA*y;
estimate = [x(1) x(2)]

% Output
plotposition(beacons, estimate, actual_pos);
error = pdist([actual_pos;estimate],'euc')
