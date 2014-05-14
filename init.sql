CREATE TABLE `video_class` (`video_id` varchar(45) NOT NULL, `class_id` int(11) NOT NULL,
 `component_id` int(11) NOT NULL, `certainty` float DEFAULT NULL, PRIMARY KEY (`class_id`,`video_id`));

CREATE TABLE `frame_class` (`id` int(11) NOT NULL AUTO_INCREMENT, `video_id` varchar(45) NOT NULL, 
	`frame_id` int(11) NOT NULL, `class_id` int(11) NOT NULL, `component_id` int(11) NOT NULL, 
	`certainty` float DEFAULT NULL, PRIMARY KEY (`id`), UNIQUE KEY `video_id` (`video_id`,`frame_id`,
		`class_id`,`component_id`));