function main()
%% Extended Kalman Filter Simulation
clear all; close all;
clc
%% system model
F = [1 0 0;
    0 0 0;
    0 0 0];

% w = (2*pi*u)/one_rotation_dis;

% B = [1*u; offset*sin(w*time) + roller*sin(9*w*time); u];

H = [1 0 0; 0 1 0; 0 0 1];

offset = -0.15;
roller = 0.08;
%% Initial Conditions
xTrue(:,1) = [0;0;0]; 
xEst(:,1) = [0;0;0]; 
xPred = [0;0;0]; 
nx = length(xPred(:,1)); % number of state
Pp = eye(nx); % guess of initial error covariance
sigma_Pp(:,1) = sqrt(diag(Pp));
z(:,1) = [0; 0; 0];
zPred(:,1) =[0; 0; 0];
sigma_w = [0.1 0.1 0.1]';
sigma_v = [0, 0.3, 0.3]';
Q = diag(sigma_w.^2);
R = diag(sigma_v.^2);

T = 1;
t = 0:T:400;
u = 0.833;
for i = 1:length(t)-1
    one_rotation_dis = 314.159;
    w = (2*pi*u)/one_rotation_dis;
    B = [1*u; offset*sin(w*i) + roller*sin(9*w*i); u];
    xTrue(:,i+1) = F*xTrue(:,i) + B;
    %ground truth
    
    ud = u + Q * randn(1,1);
    z(:,i+1) = H*xTrue(:,i);
    % observation
    
    B = [1*u; offset*sin(w*i) + roller*sin(9*w*i); u];
    xPred = F*xEst(:,i)+B;
    jF = jaco_f(xEst(:,i), u, i, offset, roller);
    PPred = jF*Pp*jF'+Q
    % predict
    jH = jaco_h()
    zPred(:,i+1) = observation_model(xPred);
    y = z(:,i+1)-zPred(:,i+1);
    S = jH*PPred*jH'+R;
    K = PPred*jH'*inv(S);
    xPred
    K
    y
    xEst(:,i+1) = xPred + K*y
    Pp = eye(length(xEst(:,i+1))-K*jH)*PPred
    
%     obs_val= observation(xTrue(:,i), u, i, offset, roller);
%     xTrue(:,i+1) = obs_val(1); z(:,i+1) = obs_val(2);
%     ud = u + input_noise * randn(1,1)
%     est_val = ekf_estimation(xEst(:,i), Pp, z(:,i), ud, i, offset, roller);
%     xEst(:,i+1) = est_val(1); Pp(:,i+1) = est_val(2);
end
figure; clf; hold on;
plot(xTrue(1,:),xTrue(2,:),'.-b');        
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

function obs_val= observation(xTrue, u, time, offset, roller)
disp("here");
xTrue = motion_model(xTrue, u, time, offset, roller);
ud = u + input_noise * randn(1,1)
z = observation_model(xTrue);
obs_val =[xTrue z];
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

function est_val = ekf_estimate(xEst, PEst, z, u, time, offset, roller, Q, R)
xPred = motion_model(xEst, u, time, offset, roller);
jf = jaco_f(xEst, u, time, offset, roller);
PPred = jf*PEst*jf'+Q ;

jh = jaco_h();
zPred = observation_model(xPred);
y = z - zPred;
S = jh*PPred*jh' + R;
K = PPred * jh' * inv(S);
xEst = xPred + K*y;
PEst = (eye(3) - K*jh)*PPred;
est_val = [xEst PEst];
end