function main()
clear all; close all;
clc
%% Extended Kalman Filter Simulation
%% system model
F = [1 0 0;
    0 0 0;
    0 0 0];

H = [1 0 0; 
    0 1 0; 
    0 0 1];

offset = -0.15;
roller = 0.08; % Wheel axis offset and roller error
% u =input ("set the feedrate (mm/min)")
u = 0.833; % feedrate
T = 1;
one_rotation_dis = 314.159;
w = (2*pi*u)/one_rotation_dis;
%% Initial Conditions
xTrue(:,1) = [0;0;0]; % true initial state
xEst(:,1) = [0;0;0]; % estimation state
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
measurement_noise = diag([0,0.1,0.1]).^2;
input_noise = diag([0.01]).^2;
t = 0:T:400;
%% KF Routine
grid on
hold on
spicemen_x = [0 400 400 0];
spicemen_y = [0 0 1 1];
tool_x = [-1.5 1.5 1.5 -1.5];
tool_y = [0.5 0.5 1.5 1.5];
axis([0 400 0 5]);
box =fill(spicemen_x, spicemen_y,'Black');
tool =fill(tool_x, tool_y,'White');
%remove_area =fill(tool_x, tool_y,'b','LineStyle','none'); %,'EraseMode','none'
    for i = 1:length(t)-1
        offset_e = -0.15 + 0+0.05*rand(1,1);
        roller_e = 0.08-0.01+0.01*rand(1,1); % Wheel axis offset and roller error
        %% time varying system
        B = [1*u; offset*sin(w*i) + roller*sin(9*w*i); u];
        %% True dynamics (observation)
        xTrue(:,i+1) = F*xTrue(:,i) + B;
            z(:,i+1) = H*xTrue(:,i) + measurement_noise *randn(3,1);
            ud = u + input_noise * randn(1,1);
            w = (2*pi*u)/one_rotation_dis;
            Bd = [1*ud; offset_e*sin(w*i) + roller_e*sin(9*w*i); ud];
        %% Time update
        %% (ekf estimation) (xEst, PEst, z, ud, time, offset, roller)
        %% Prediction of state
        xPred = F*xEst(:,i)+Bd;
        jF = jaco_f(xEst(:,i), ud, i, offset_e, roller_e);
        %% Prediction of covariance
        Pp = jF*Pp*jF'+Q;
        jH = jaco_h();
        %% Measurement generation
        %% input noise and measurement update
        %% Compute jacobian
        zPred(:,i+1) = observation_model(xPred); %+ sigma_v;
        y = z(:,i+1)-zPred(:,i+1);
        %% Innovation Covariance
        S = jH*Pp*jH'+R;
        %% Kalman gain
        K = Pp*jH'*inv(S);
        %% State update
        xEst(:,i+1) = xPred + K*y;
        %% Covariance update
        Pp = (eye(length(xEst(:,i+1)))-K*jH)*Pp;
        x_t = xEst(1,:);
        z_t = xEst(2,:);
        if i >1
            tempX = [(x_t(i-1)-1.5) (x_t(i-1)+1.5) (x_t(i-1)+1.5) (x_t(i-1)-1.5)];
            tempY = [0.75+z_t(i-1) 0.75+z_t(i-1) z_t(i-1)+1.5 z_t(i-1)+1.5];
            remove_area =fill(tempX, tempY,'White','LineStyle','none'); %,'EraseMode','none'
            set(remove_area,'Xdata', tempX, 'Ydata', tempY);
        end
        tool_x = [(x_t(i)-1.5) (x_t(i)+1.5) (x_t(i)+1.5) (x_t(i)-1.5)];
        tool_z = [0.75+z_t(i) 0.75+z_t(i) z_t(i)+1.5 z_t(i)+1.5];

        updateX = tool_x;
        updateY = tool_z;
        set(tool,'Xdata', updateX, 'Ydata', updateY);
        drawnow;
        plot(xTrue(1,:)-1.5,0.75,'.-g'); 
        plot(xTrue(1,:),0.75+xTrue(2,:),'.-b');    
        plot(xEst(1,:),0.75+xEst(2,:),'.-r');
%         plot(z(1,:),0.75+z(2,:),'.-y');
    %     pause(0.2)
    %     end
    end
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
v = x(3);
w = 2*pi*u/one_rotation_dis;
jf = [ 1.0, 0.0, 1;
    0 0 (2*pi*time/one_rotation_dis)*cos(w*time)+(v*roller/9/offset)*cos(v*i/9/offset);
    0 0 1];
end

function jh = jaco_h()
jh = [ 1, 0, 0;
    0 1 0;
    0 0 1];
end