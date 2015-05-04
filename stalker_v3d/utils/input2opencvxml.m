function input2opencvxml(X, Y)
% A util script to save the input vectors and labels to openCV compatible xml format
% X input feature matrix of size #sbj * #feat
% Y label vector of size #sbj * 1

addpath('.');
docNode = com.mathworks.xml.XMLUtils.createDocument('opencv_storage');
opencvStorageNode = docNode.getDocumentElement();

% Data
datamatNode = docNode.createElement('datamat');
datamatNode.setAttribute('type_id', 'opencv-matrix');
opencvStorageNode.appendChild(datamatNode);

rowsNode = docNode.createElement('rows');
colsNode = docNode.createElement('cols');
dtNode = docNode.createElement('dt');
dataNode = docNode.createElement('data');

rowsNode.appendChild(docNode.createTextNode(num2str(size(X, 1))));
colsNode.appendChild(docNode.createTextNode(num2str(size(X, 2))));
dtNode.appendChild(docNode.createTextNode('f'));
sX = mat2str(X);
sX = strrep(sX, '[', '');
sX = strrep(sX, ']', '');
sX = strrep(sX, ';', sprintf('\n'));
dataNode.appendChild(docNode.createTextNode(sX));

datamatNode.appendChild(rowsNode);
datamatNode.appendChild(colsNode);
datamatNode.appendChild(dtNode);
datamatNode.appendChild(dataNode);

% Label
labelmatNode = docNode.createElement('labelmat');
labelmatNode.setAttribute('type_id', 'opencv-matrix');
opencvStorageNode.appendChild(labelmatNode);

rowsNode = docNode.createElement('rows');
colsNode = docNode.createElement('cols');
dtNode = docNode.createElement('dt');
dataNode = docNode.createElement('data');

rowsNode.appendChild(docNode.createTextNode(num2str(size(Y), 1)));
colsNode.appendChild(docNode.createTextNode(num2str(size(Y), 2)));
dtNode.appendChild(docNode.createTextNode('f'));
sY = mat2str(Y);
sY = strrep(sY, '[', '');
sY = strrep(sY, ']', '');
sY = strrep(sY, ';', ' ');
dataNode.appendChild(docNode.createTextNode(sY));

labelmatNode.appendChild(rowsNode);
labelmatNode.appendChild(colsNode);
labelmatNode.appendChild(dtNode);
labelmatNode.appendChild(dataNode);

% --- Extract the path without extension and make new name for the xml
path2save = fullfile(pwd, 'mat2opencvdata.xml');
xmlwrite(path2save, docNode);

end