Classes:
- GaussianMixtureModel: (a variant, actually) implementation of the Detection interface.

Files:
- gaussian_mixture_model.*: header and implementation files for the GaussianMixtureModel class

Dependencies:
- CvBlob
- SWComponent

---------------

Changelog:

06-04-2011 v0.2.1
- Implemented SWComponent methods.

05-04-2011 v0.2.0
- Changed Detection and GaussianMixtureModel to use CvBlob.

04-04-2011 v0.1.0
- Changed name from ConcettoDetection to GaussianMixtureModel.
- Fixed bug in the blob evaluation (delay in the complete detection of a fish).
- Added 5 pixel margin to mask sides.

22-03-2011 v0.0.3
- Masks are properly computed.

18-03-2011 v0.0.2
- Fixed memory leaks (hopefully).

16-03-2011 v0.0.1
- First version of all classes
