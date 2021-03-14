function main()
%% Extended Kalman Filter Simulation
clear all; close all;
clc
%% system model
F = [1 0 0;
    0 0 0;
    0 0 0];

H = [1 0 0; 0 1 0; 0 0 1];

offset = -0.15;
roller = 0.08; % Wheel axis offset and roller error
u = 0.833; % feedrate
T = 1;
one_rotation_dis = 314.159;
w = (2*pi*u)/one_rotation_dis;
%% Initial Conditions
xTrue(:,1) = [0;0;0]; % true initial state
xEst(:,1) = [0;-0.3;0]; % estimation state
xPred = [0;0;0]; %initial posteriori estimation
nx = length(xPred(:,1)); % number of state
Pp = eye(nx); % guess of initial error covariance
sigma_Pp(:,1) = sqrt(diag(Pp));

z(:,1) = [0; 0; 0]; 
zPred(:,1) =[0; 0; 0];

%% Noise
sigma_w = [0.1 0.1 0.1]'; % system noise (std of x, z, vel)
sigma_v = [0, 0.3, 0.3]'; % measurement noise 
Q = diag(sigma_w.^2);
R = diag(sigma_v.^2);

t = 0:T:400;
%% KF Routine
for i = 1:length(t)-1
    %% time varying system
    B = [1*u; offset*sin(w*i) + roller*sin(9*w*i); u];
    %% True dynamics
    xTrue(:,i+1) = F*xTrue(:,i) + B + sigma_w;
    %% Time update
    %% Prediction of state
    xPred = F*xEst(:,i)+B;
    jF = jaco_f(xEst(:,i), u, i, offset, roller);
    %% Prediction of covariance
    PPred = jF*Pp*jF'+Q
    %% Measurement generation
    %% input noise and measurement update
    ud = u + Q * randn(1,1);
    z(:,i+1) = H*xTrue(:,i);
    %% Compute jacobian
    jH = jaco_h()
    zPred(:,i+1) = observation_model(xPred)+ sigma_v;
    y = z(:,i+1)-zPred(:,i+1);
    %% Innovation Covariance
    S = jH*PPred*jH'+R;
    %% Kalman gain
    K = PPred*jH'*inv(S);
    %% State update
    xEst(:,i+1) = xPred + K*y
    %% Covariance update
    length(xEst(:,i+1))
    Pp = (eye(length(xEst(:,i+1)))-K*jH)*PPred
end
figure; clf; hold on;
plot(xTrue(1,:),xTrue(2,:),'.-b');    
plot(xEst(1,:),xEst(2,:),'.-r');    
plot(zPred(1,:),zPred(2,:),'.-g');    
xlabel('x, m'); ylabel('z, m')
end
%% Plot: position estimate x_1

function x = motion_model(x,u,time, offset, roller)
one_rotation_dis = 314.159;
F = [1.0 0 0;
    0 0 0;
    0 0 0];
w = (2*pi*u)/one_rotation_dis;
B = [1*u; offset*sin(w*time) + roller*sin(9*w*time); u];
x = F*x + B;
end

%% partial_h/partial_X: Jacobian of measurement eq
function z = observation_model(x)
H = [1 0 0; 0 1 0; 0 0 1];
z = H*x;
end

function jf = jaco_f(x, u, time, offset, roller)
one_rotation_dis = 314.159;
u = x(3);
w = 2*pi*u/one_rotation_dis;
jf = [ 1.0, 0.0, 1;
    0 0 (2*pi*time/one_rotation_dis)*cos(w*time)+(u*roller/9/offset)*cos(u/9/offset);
    0 0 1];
end

function jh = jaco_h()
jh = [ 1, 0, 0;
    0 1 0;
    0 0 1];
end