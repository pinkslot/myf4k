function [classification, frame_classification, class] = test_png(video_path, classification_lib, images_dir, frames, CX, training, sift_radius)

if nargin < 7
    sift_radius = 6;
end

try
    % class
    class = '';
    % video classification
    classification(length(training)) = struct('name','','num_correct',0);
    % frame classification
    frame_classification = struct('frameNr', '', 'classes', cell(0));
    % get video info
    get_info_command = ['. ' classification_lib ' ; get_video_info ', video_path];
    [status, video_info] = system(get_info_command);
    if status == 0
        % training data
        traindata = [];
        % get properties
        idx = strfind(video_info, ' ');
        frameRate = str2num(video_info(1 : idx-1));
        videoFrames = str2num(video_info(idx+1 : end));
        if (videoFrames >= frames)
            % part is the step
            part = floor(videoFrames/frames);
        else
            part = 1;
        end
        % convert to png
        fprintf('Converting video %s to png\n', video_path);
        convert_command = ['ffmpeg -i ', video_path, ' -vframes ', num2str(frames), ' -filter:v select="not(mod(n\,', num2str(part), '))",setpts="N/(', num2str(frameRate), '*TB)" ', images_dir, '/output%d.png >/dev/null 2>&1'];
        system(convert_command);
        fprintf('Video %s converted\n', video_path);
        % Start frame
        frameNr = 0;
        % index
        k = 1;
        images = dir(images_dir);
        % Hidden is a variable containing the number of hidden files
        if (strcmp(images(3).name, '.DS_Store'))
           hidden = 3;
        else 
           hidden = 2;
        end
	images_num = length(images) - hidden;
	if images_num < frames
		fprintf('Number of images  < %d\n\n', frames);
		return
	end
	% for each video frame converted
        while k  <= images_num 
            % add this frameNr to frame classification struct
            frame_classification(k).frameNr = frameNr;
            % get frame
            data = imread([images_dir, '/output', num2str(k) , '.png']);
            if k == 1
                height = size(data, 1);
                width = size(data, 2);
            end
            % calculate time
            time = frameNr/frameRate;
            traindata = reanalyze_video(data,width,height,frameNr,time,CX,traindata,sift_radius);
            frameNr = frameNr + part;
            k = k + 1 ;
        end
	        
        classified_frames = 0;
	
	for i = 1 : length(training)
            output = svmclassify(training(i).svm,traindata);
            % get the indices of frames classified as training(i).name
            frame_indices = find(output>0);
            num_output = size(frame_indices,1);
            % remove whitespaces from class name
            idx = strfind(training(i).name,' ');
            if(isempty(idx))
                classification(i).name = training(i).name;
            else
                classification(i).name = training(i).name(1:idx(1)-1);
            end
            classification(i).num_correct = num_output;
            classified_frames = classified_frames + num_output;
            % for each index in frame_indices add classification(i).name to the
            % respective frame in frame_classification struct
            for j = frame_indices'
                frame_classes = frame_classification(j).classes;
                next_frame_classes_idx = length(frame_classes) + 1;
                frame_classification(j).classes(next_frame_classes_idx) = {classification(i).name};
            end
        end

        for i = 1 : length(classification)
            if(strcmp(classification(i).name,'Encoding'))
                if(classification(i).num_correct/classified_frames >= 0.25 && classified_frames > 2)
                    class = 'Encoding';
                end
            end
        end

        if(~strcmp(class,'Encoding'))
            max_correct = max([classification.num_correct]);
            if(max_correct==0)
                class = 'Unknown';       
            else
                [value,index] = max([classification.num_correct]);
                class = classification(index).name;
            end
        end

        % if a frame has not been classified set to Unknown
        for i = 1 : length(frame_classification)
            if isempty(frame_classification(i).classes)
                frame_classification(i).classes(1) = {'Unknown'};
            end
        end
    else
        fprintf('ffmpeg can''t open video %s\n\n', video_path);
    end
    
catch err
    disp(err.message);
    rethrow(err);
end
