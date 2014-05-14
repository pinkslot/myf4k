# The external_libs.mk file allows to specify -I and -L arguments for external libraries, i.e.
# required by the project but not included in the 3rdParty directory.
# Since those settings are strongly machine-dependent, NO external_libs.mk FILE IS INCLUDED IN THE
# REPOSITORY! Instead, this file (external_libs.mk.default) should be renamed and edited as appropriate.
# The resulting external_libs.mk file SHOULD NOT BE ADDED TO THE REPOSITORY.
# Below is the list of libraries which can be configured.

# OpenCV
OPENCV_INCLUDES := -I/usr/local/include/opencv -I/usr/local/include/opencv2
OPENCV_LIB_INCLUDES := -I/mnt/fst/cw/OpenCV-2.4.2/include/opencv2 -I/mnt/fst/cw/OpenCV-2.4.2/include/opencv
OPENCV_LIBS := -L/usr/local/lib

# MySQL connector
MYSQL_CONNECTOR_INCLUDES := -I/usr/local/include/cppconn
MYSQL_CONNECTOR_LIB_INCLUDES := -I/usr/local/lib

# Boost
BOOST_INCLUDES := -I/usr/include/boost
BOOST_LIB_INCLUDES := -I/usr/lib
