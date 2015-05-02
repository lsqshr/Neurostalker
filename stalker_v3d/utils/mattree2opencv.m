function matrtree2opencv(matrtreepath)
% Convert .mat file contains a single random forest tree to opencv
% compatible xml file
% WIP

matrtree = load(matrtreepath, 't');
matrtree = matrtree.t;

% --- Top layer of the xml
docNode = com.mathworks.xml.XMLUtils.createDocument('opencv_storage');
opencvStorageNode = docNode.getDocumentElement();
mlrtreesNode = docNode.createElement('opencv_ml_rtrees');
opencvStorageNode.appendChild(mlrtreesNode);

% --- Second Layer under opencv_ml_rtrees
formatNode = docNode.createElement('format');
formatNode.appendChild(docNode.createTextNode('3'));
mlrtreesNode.appendChild(formatNode);

isclassifierNode = docNode.createElement('is_classifier');
isclassifierNode.appendChild(docNode.createTextNode(num2str(matrtree.is_classifier)));
mlrtreesNode.appendChild(isclassifierNode);

% TODO: DH, pls help me check the meaning of these parameters
% For the parameters, you may find this helpful 
% http://docs.opencv.org/2.4.9/modules/ml/doc/decision_trees.html
varallNode = docNode.createElement('var_all');
varallNode.appendChild(docNode.createTextNode(num2str(matrtree.var_count+1)));
mlrtreesNode.appendChild(varallNode);

varcountNode = docNode.createElement('var_count');
varcountNode.appendChild(docNode.createTextNode(num2str(matrtree.var_count)));
mlrtreesNode.appendChild(varcountNode);

ordvarcountNode = docNode.createElement('ord_var_count'); % Seems for regression all var should be ordered
% http://stackoverflow.com/questions/24327230/using-opencv-random-forest-for-regression
ordvarcountNode.appendChild(docNode.createTextNode(num2str(matrtree.var_count+1)));
mlrtreesNode.appendChild(ordvarcountNode);

catvarcountNode = docNode.createElement('cat_var_count');
catvarcountNode.appendChild(docNode.createTextNode(num2str(0)));
mlrtreesNode.appendChild(varcountNode);

% --- Third layer training_params: Keep them as 0 for now, since we only
% use the saved file for prediction making
trainingParamsNode = docNode.createElement('traning_params');
mlrtreesNode.appendChild(trainingParamsNode);

usesurrogatesNode = docNode.createElement('use_surrogates');
usesurrogatesNode.appendChild(docNode.createTextNode('0'));
trainingParamsNode.appendChild(usesurrogatesNode);

maxcategoriesNode = docNode.createElement('max_categories');
maxcategoriesNode.appendChild(docNode.createTextNode('0'));
trainingParamsNode.appendChild(maxcategoriesNode);

regressionaccuracyNode = docNode.createElement('regression_accuracy');
regressionaccuracyNode.appendChild(docNode.createTextNode('0'));
trainingParamsNode.appendChild(regressionaccuracyNode);

maxdepthNode = docNode.createElement('max_depth');
maxdepthNode.appendChild(docNode.createTextNode('0'));
trainingParamsNode.appendChild(maxdepthNode);

minsamplecountNode = docNode.createElement('min_sample_count');
minsamplecountNode.appendChild(docNode.createTextNode('0'));
trainingParamsNode.appendChild(minsamplecountNode);

crossvalidationfoldsNode = docNode.createElement('cross_validation_folds');
crossvalidationfoldsNode.appendChild(docNode.createTextNode('0'));
trainingParamsNode.appendChild(crossvalidationfoldsNode);

nactivevarsNode = docNode.createElement('nactive_vars');
nactivevarsNode.appendChild(docNode.createTextNode('0'));
trainingParamsNode.appendChild(nactivevarsNode);
% --- End Third layer training_params

globalvaridxNode = docNode.createElement('global_var_idx');
globalvaridxNode.appendChild(docNode.createTextNode('0'));
mlrtreesNode.appendChild(globalvaridxNode);

varidxNode = docNode.createElement('var_idx');
varidxNode.appendChild(docNode.createTextNode(num2str(1:matrtree.var_count)));
mlrtreesNode.appendChild(varidxNode);

vartypeNode = docNode.createElement('var_type');
vartypeNode.appendChild(docNode.createTextNode(num2str(zeros(1,matrtree.var_count))));
mlrtreesNode.appendChild(vartypeNode);

catofsNode = docNode.createElement('cat_ofs');
catofsNode.appendChild(docNode.createTextNode('0 0 0 0')); % Not sure what it is for now
mlrtreesNode.appendChild(catofsNode);

% This might not be used for regresssion trees
%classlabelsNode = docNode.createElement('class_labels');
%classlabelsNode.appendChild(docNode.createTextNode('1 2')); 
%mlrtreesNode.appendChild(classlabelsNode);

missingsubstNode = docNode.createElement('missing_subst');
missingsubstNode.appendChild(docNode.createTextNode(num2str(zeros(1,matrtree.var_count))));
mlrtreesNode.appendChild(missingsubstNode);

% Out-of-bag permutations were not saved. Run with 'oobvarimp' set to 'on'.
% in matlab 
% ooberrorNode = docNode.createElement('oob_error'); 
% ooberrorNode.appendChild(docNode.createTextNode('0.')); % Not saved
% mlrtreesNode.appendChild(ooberrorNode);

ntreesNode = docNode.createElement('ntrees');
ntreesNode.appendChild(docNode.createTextNode(num2str(matrtree.ntrees))); % Done
mlrtreesNode.appendChild(ntreesNode);

treesNode = docNode.createElement('trees');
% --- Start the tree
for i = 1 : matrtree.trees
    treeNode = docNode.createElement('_');
    nodesNode = docNode.createElement('nodes');
    for j = 1 : NumNodes
        % Things get a bit complicated here, matlab saves the tree in a
        % different manner regarding the openCV
        % Matlab saves the topology by stating the node IDs in an array
        % The implementation of writeTree in OpenCV - https://github.com/Itseez/opencv/blob/d7cb99254eb4e545bfe9c2d6bed1a2adbe30a547/modules/ml/src/tree.cpp#L1649
        
    end
    treeNode.appendChild(nodesNode);
    treesNode.appendChild(treeNode);
end

% --- Extract the path without extension and make new name for the xml
[pathstr, name, ~] = fileparts(matrtreepath);
opencvrtreepath = fullfile(pathstr, strcat(name,'.xml'));

xmlwrite(opencvrtreepath, docNode);
end