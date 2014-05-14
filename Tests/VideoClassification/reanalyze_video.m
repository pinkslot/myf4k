function traindata = reanalyze_video(data,width,height,frameNr,time,CX,traindata,sift_radius)
% processFrame(data,width,height,frameNr)
%
% This is the function prototype to be used by the matlabCommand option of
% mmread.
% INPUT
%   data        the raw captured frame data, the code below will put it
%               into a more usable form
%   width       the width of the image
%   height      the height of the image
%   frameNr     the frame # (counting starts at frame 1)
%   time        the time stamp of the frame (in seconds)
%   CX
%   traindata
%   sift_radius


if nargin < 8
    sift_radius = 6;
end

try
    data = imresize(data, [240 320]);
%     image(data)
%     title(['Processing frame ' num2str(frameNr) ' ' num2str(time) 's ....']);
%     drawnow;
    %pause(0.1);
    im = rgb2gray(data);
    sigma = 4; 
    radius = 3;
    display = 0;
    [cim, r, c] = harris(im, sigma, radius, display);
    circles = zeros([size(c,1),3]);
    circles(:,1) = c;
    circles(:,2) = r;
    circles(:,3) = sift_radius*ones([size(c,1),1]);
    sift_arr = find_sift(im, circles, 1.5); 
    n2 = dist2(CX,sift_arr);
    % [C,I] = min(...) finds the indices of the minimum values 
    [H,I] = min(n2);
    mx = 1:1:500;
    n = hist(I,mx);
    n = n./sum(n);
    traindata = [traindata;n];
catch
    % if we don't catch the error here we will loss the stack trace.
    err = lasterror;
    disp([err.identifier ': ' err.message]);
    rethrow(err);
end
