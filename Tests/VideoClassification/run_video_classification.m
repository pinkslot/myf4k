function run_video_classification(video_id, video_path, classification_lib, images_dir, CX, training, frames, classes_map, component_id, sift_radius);

if nargin < 10
    sift_radius = 6;
end

query_frame_class_base = sprintf('INSERT INTO frame_class(video_id, frame_id, class_id, component_id, certainty) ');
fprintf('Classifying %s\n', video_id);
[classification, frame_classification, class] = test(video_path, classification_lib, images_dir, frames, CX, training, sift_radius);
if ~isempty(class)
    out_str = strcat('Video ', video_id, ' is classified as:  ', class);
    fprintf('%s\n', out_str);
    % insert into database frame classification
    for j = 1 : length(frame_classification)
        frame_id = frame_classification(j).frameNr;
        frame_classes = frame_classification(j).classes;
        for k = 1 : length(frame_classes)
            frame_class = char(frame_classes(k));
            % get frame class id
            class_id = classes_map(frame_class).class_id;
            certainty = classes_map(frame_class).certainty;
            query = sprintf(' values(''%s'', ''%d'', ''%d'', ''%d'', ''%0.2f'' ) ', video_id, frame_id, class_id, component_id, certainty);
            query_frame_class = strcat(query_frame_class_base, query);
            query_frame_already_classified = sprintf('SELECT class_id from frame_class where video_id = ''%s'' and frame_id = ''%d'' and class_id = %d and component_id = ''%d''', video_id, frame_id, class_id, component_id);
            class_id = mysql(query_frame_already_classified);
            % if this frame has not been inserted into database
            if isempty(class_id)
                try
                    mysql(query_frame_class);
                    fprintf('Record inserted into frame_class\n');
                catch err
                    disp(err.message);
                end
            else
		fprintf('Record already exists in frame_class\n');    
	    end
        end
    end
    % get video class info
    class_id = classes_map(class).class_id;
    certainty = classes_map(class).certainty;
    query_video_class = sprintf('INSERT INTO video_class(video_id, class_id, component_id, certainty) values(''%s'', ''%d'', ''%d'', ''%0.2f'') ', video_id, class_id, component_id, certainty);
    try
        mysql(query_video_class);
        fprintf('Record inserted into video_class\n\n');
    catch err   
        disp(err.message);
    end
end


