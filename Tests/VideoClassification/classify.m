function classify(training, classes_map, classification_lib, video_file_format, varargin)
    setenv('PATH','/pkg/mpi/gcc/4.6.1/openmpi-1.6.1/bin:/home/u00f4k00/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/X11R6/bin:/usr/games:/opt/kde3/bin:/usr/lib/mit/bin:/usr/lib/mit/sbin:/home/u00f4k00/work/swipl-5.10.2/bin/x86_64-linux:/pkg/math/matlab/bin/:/pkg/lsf/cluster5/7.0/linux2.6-glibc2.3-x86_64/bin:/pkg/lsf/cluster5/7.0/linux2.6-glibc2.3-x86_64/etc:/pkg/lsf/cluster5/perf/1.2/bin:/pkg/lsf/cluster5/gui/2.0/bin:/bin:/usr/bin:/usr/local/bin:/home/u00f4k00/bin:/pkg/user/u00f4k00/simone/ffmpeg:/pkg/user/u00f4k00/francesca/local/bin')

    user = '';
    password = '';
    dbName = ''; 
    host = '.nchc.org.tw';
    port = 3306;
    
    try
        mysql('open', [host, ':', num2str(port)] , user, password);
    	mysql('use', dbName);

    	load(training)
    	load(classes_map)
        frames = 20;
        sift_radius = 6;
        component_id = 74;
        idx = strfind(video_file_format, '%s');
        video_path_base = video_file_format(1:idx-1);
        video_ext = video_file_format(idx+2:end);
        for i = 1:nargin-4
            video_id = varargin{i};
            fprintf('Video %s\n', video_id);
            video_path = [video_path_base, video_id, video_ext];
            fprintf('Video path: %s\n', video_path);
            % If the video has not been classified
            query_video_already_classified = sprintf('SELECT class_id from video_class where video_id = ''%s'' and component_id = ''%d''', video_id, component_id);
            class_id = mysql(query_video_already_classified);
            if(isempty(class_id))
                images_dir = [video_id,'_images'];
                if  ~exist(images_dir)
                    fprintf('Creating directory %s\n', images_dir);
                    mkdir(images_dir);
                    run_video_classification(video_id, video_path, classification_lib, images_dir, CX, training, frames, classes_map, component_id, sift_radius);
                    rmdir(images_dir, 's');
                else
                    fprintf('Directory %s already exists\n\n', images_dir);
                end
            else
                fprintf('Video %s already in database\n\n', video_id);
            end
        end
    	mysql('close')
    catch err
        disp([err.identifier ': ' err.message]);
        for i = 1:length(err.stack)
            disp([err.stack(i).file ' (' err.stack(i).name ') Line: ' num2str(err.stack(i).line)]);
        end
    end
end

