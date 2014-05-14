./ada_boost_train -o  ./ -t 30 -c 100 \
	-g blobs1/masks/%d.png -i blobs1/images/%d.png \
	-g blobs3/masks/%d.png -i blobs3/images/%d.png \
	-g blobs5/masks/%d.png -i blobs5/images/%d.png \
	-g blobs10/masks/%d.png -i blobs10/images/%d.png \
	-g blobs14/masks/%d.png -i blobs14/images/%d.png \
	-g blobs29/masks/%d.png -i blobs29/images/%d.png \
	-g blobs34/masks/%d.png -i blobs34/images/%d.png
