Classes:
- TrackingEvaluator: abstract class which defines methods for the evaluation of a tracking algorithm.
- ScoreHistory: class which represents the evaluation history of a tracked object.
- Score: class which represents a score for a given object in a given frame

Files:
- tracking_evaluator.h/cpp: header and (partial) implementation files for the TrackingEvaluator class
- score_history.h/cpp: header and implementation files for the TrackedObjectScore class
- score.h: header file for the ScoreHistory class

Dependencies: TrackedObject

---------------

Last update:	Simone, 26-05-2011

Changelog:

26-05-2011 v0.0.2
- Changed history class to ScoreHistory.

18-05-2011 v0.0.1
- First version
