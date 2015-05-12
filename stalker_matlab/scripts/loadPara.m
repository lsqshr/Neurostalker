%------ PARA
% TEST
TESTSIZE        = 1;
TESTSTARTLOC    = [[89, 363, 30]];
TESTSTARTDIR    = [[-0.9278, 0.1916]];
NTESTSTEP       = 100;
STEPSIZE        = 1;
NOISETYPE       = 'original'; % The noise added to vision blocks: original, gauss, salt_pepper
% General Learning
DEBUG           = true; % DEBUG=true will only train models with 1000 cases
FRAMEWORK       = 'NORMAL' % The framework used for training/walking the neurostalker: 'NORMAL'/PUFFER
PREFIX          = 'OP_';
CACHETRAINDATA  = false;
CACHETRAINMODEL = false;
RUNTEST         = false;
VBSIZE          = 13; % Predefined size of the vision box
% Random Forest 
RF.NTREE        = 200;
RF.MTRY         = 140;
% Neural Network
NN.NHLAYER      = 2;
NN.NHNEURON     = 13 * 5;
NN.BATCHSIZE    = 1000;
NN.SAEEPOCH     = 50;
NN.NNEPOCH      = 100;
NN.ZEROMASK     = 0.5;
% ENDPARA