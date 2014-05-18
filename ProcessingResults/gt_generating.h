// Generate GroundTruth
#ifndef GT_GENERATING_H
#define GT_GENERATING_H

#include <context.h>
#include <results.h>
#include <db_helper.h>
#include <db_connection.h>

class GtGenerating
{
private:
	// fields
	Context context;
	db::DBConnection db_conn;	
	pr::Results fish_results, nofish_results;

	// methods
	void on_mouse(int evt, int x, int y, int flags, void* param);

public:
	inline GtGenerating(Context &c, db::DBConnection &d): context(c), db_conn(d)
	{
	}
	void nextFrame();
	void saveResult();
};

#endif
