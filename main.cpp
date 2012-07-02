/*  
 * This file is part of assimp2ctm.
 *
 * assimp2ctm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * assimp2ctm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with assimp2ctm.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "common.h"
#include "glwindow.h"
#include "export.h"

class ConvWindow : public Fl_Window
{
public:
	 ConvWindow (int W, int H, const char *L=0);
	 int handle (int event);
private:
	 int startx, starty;
};

ConvWindow::ConvWindow (int W, int H, const char *L)
	: Fl_Window (W, H, L)
{
}

ConvWindow *window = 0;
Fl_Wizard *wizard = 0;
Fl_Input *filename = 0;
Fl_Input *export_filename = 0;
GlWindow *glwin = 0;
Fl_Simple_Counter *mesh_counter = 0;
Fl_Simple_Counter *level_counter = 0;
Fl_Round_Button *compression_raw_button = 0;
Fl_Round_Button *compression_mg1_button = 0;
Fl_Round_Button *compression_mg2_button = 0;

extern std::vector<Mesh> meshes;
extern LogStream logstream;

int ConvWindow::handle (int event)
{
	int x, y;
	switch (event)
	{
	case FL_MOUSEWHEEL:
		glwin->zoom (Fl::event_dy ());
		glwin->redraw ();
		redraw ();
		break;
	case FL_DRAG:
		x = Fl::event_x () - startx;
		y = Fl::event_y () - starty;
		glwin->rotate (x, y);
		glwin->redraw ();
		redraw ();
	case FL_PUSH:
		startx = Fl::event_x ();
		starty = Fl::event_y ();
		break;
	}
	return Fl_Window::handle (event);
}

void load_cb (Fl_Widget *w, void *u)
{
	if (LoadMeshes (filename->value ()))
	{
		glwin->DrawMesh (&meshes[0]);
		mesh_counter->value (0.0);
		glwin->reset ();

		window->resize (window->x (), window->y (), 640, 480);
		wizard->next ();

		window->redraw ();
	}
}

void back_to_first_cb (Fl_Widget *w, void *u)
{
	window->resize (window->x (), window->y (), 400, 80);
	wizard->prev ();
}

void done_cb (Fl_Widget *w, void *u)
{
	if (export_mesh (export_filename->value (),
									 &meshes[int (mesh_counter->value ())]))
	{
		exit (0);
	}
}

void choose_cb (Fl_Widget *w, void *u)
{
	Fl_Native_File_Chooser chooser;
	chooser.title ("Load a model to convert");
	chooser.type (Fl_Native_File_Chooser::BROWSE_FILE);
	chooser.preset_file (filename->value ());
	switch (chooser.show ())
	{
	case -1:
		fl_message_title ("ERROR");
		fl_alert ("%s\n", chooser.errmsg ());
		break;
	case 1:
		break;
	default:
		if (chooser.filename ())
			 filename->value (chooser.filename ());
		else
			 filename->value ("NULL");
		break;
	}
}

void choose_export_cb (Fl_Widget *w, void *u)
{
	Fl_Native_File_Chooser chooser;
	chooser.title ("Save the mesh");
	chooser.type (Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	chooser.preset_file (export_filename->value ());
	switch (chooser.show ())
	{
	case -1:
		fl_message_title ("ERROR");
		fl_alert ("%s\n", chooser.errmsg ());
		break;
	case 1:
		break;
	default:
		if (chooser.filename ())
			 export_filename->value (chooser.filename ());
		else
			 export_filename->value ("NULL");
		break;
	}
}

LogStream logstream;

void counter_cb (Fl_Widget *w, void *u)
{
	Fl_Valuator *val;
	val = dynamic_cast<Fl_Valuator*> (w);
	if (val)
	{
		if (val->value () < 0.0)
			 val->value (0.0);
		if (val->value () > double (meshes.size () - 1))
			 val->value (meshes.size () - 1);

		glwin->DrawMesh (&meshes[int (val->value ())]);
		glwin->reset ();
		glwin->redraw ();
	}
}

void export_cb (Fl_Widget *w, void *u)
{
	compression_raw_button->value (0);
	compression_mg1_button->value (1);
	compression_mg2_button->value (0);
	level_counter->value (1);

	window->resize (window->x (), window->y (), 400, 255);
	wizard->next ();
	
	window->redraw ();
}

void back_to_view_cb (Fl_Widget *w, void *u)
{
	glwin->reset ();

	window->resize (window->x (), window->y (), 640, 480);
	wizard->prev ();
	
	window->redraw ();
}

void compression_raw_cb (Fl_Widget *w, void *u)
{
	export_set_compression_method (COMPRESSION_METHOD_RAW);
}

void compression_mg1_cb (Fl_Widget *w, void *u)
{
	export_set_compression_method (COMPRESSION_METHOD_MG1);
}

void compression_mg2_cb (Fl_Widget *w, void *u)
{
	export_set_compression_method (COMPRESSION_METHOD_MG2);
}


void compression_level_cb (Fl_Widget *w, void *u)
{
	Fl_Valuator *val;
	val = dynamic_cast<Fl_Valuator*> (w);
	if (val)
	{
		if (val->value () < 0.0)
			 val->value (0.0);
		if (val->value () > 9.0)
			 val->value (9.0);
		export_set_compression_level (int (val->value ()));
	}
}

int main (int argc, char *argv[])
{
	Assimp::DefaultLogger::create ("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get ()->attachStream (&logstream,
																							 Assimp::Logger::DEBUGGING
																							 |Assimp::Logger::INFO
																							 |Assimp::Logger::ERR
																							 |Assimp::Logger::WARN);

#if !defined(WIN32) && !defined (__APPLE__)
	Fl_File_Icon::load_system_icons ();
#endif

	window = new ConvWindow (400, 80, "Convert model");
	wizard = new Fl_Wizard (0, 0, 640, 480);

	// page 1
	{
		Fl_Group *g = new Fl_Group (0, 0, 400, 80);
		filename = new Fl_Input (80, 10, window->w () - 100, 25, "Filename: ");
		filename->value (".");
		Fl_Button *button  = new Fl_Button (window->w() - 80, 10,
																				70, 25, "Load");
		button->callback (choose_cb);

		Fl_Button *next = new Fl_Button (290, 45, 100, 25, "Next @->");
		next->callback (load_cb);
		g->end ();
	}
	// page 2
	{
		Fl_Group *g = new Fl_Group (0, 0, 640, 480);

		glwin = new GlWindow (5, 5, 630, 400);

		mesh_counter = new Fl_Simple_Counter (5, 410, 200, 25, "Mesh");
		mesh_counter->step (1.0);
		mesh_counter->type (FL_INT_INPUT);
		mesh_counter->tooltip ("Current mesh");
		mesh_counter->callback (counter_cb);

		Fl_Button *back = new Fl_Button(420, 445, 100, 25, "@<- Back");
		back->callback(back_to_first_cb);

		Fl_Button *next = new Fl_Button (530, 445, 100, 25, "Next @->");
		next->callback (export_cb);
		g->end ();
	}
	// page 3
	{
		Fl_Group *g = new Fl_Group (0, 0, 400, 255);

		{
			Fl_Group *g = new Fl_Group (10, 10, 380, 120);
			g->box(FL_THIN_UP_FRAME);
			{
				compression_raw_button = new Fl_Round_Button (20, 20, 180, 30,
																											"No Compression");
				compression_raw_button->type (102);
				compression_raw_button->down_box (FL_ROUND_DOWN_BOX);
				compression_raw_button->callback (compression_raw_cb);
			}
			{
				compression_mg1_button = new Fl_Round_Button (20, 50, 180, 30,
																											"MG1 Compression");
				compression_mg1_button->type (102);
				compression_mg1_button->down_box (FL_ROUND_DOWN_BOX);
				compression_mg1_button->callback (compression_mg1_cb);
				compression_mg1_button->value (1);
			}
			{
				compression_mg2_button = new Fl_Round_Button (20, 80, 180, 30,
																											"MG2 Compression");
				compression_mg2_button->type (102);
				compression_mg2_button->down_box (FL_ROUND_DOWN_BOX);
				compression_mg2_button->callback (compression_mg2_cb);
			}
			g->end ();
		}

		level_counter = new Fl_Simple_Counter (10, 140, 220, 25,
																					 "Compression Level");
		level_counter->step (1.0);
		level_counter->type (FL_INT_INPUT);
		level_counter->tooltip ("Compression Level");
		level_counter->callback (compression_level_cb);

		export_filename = new Fl_Input (10, 180, window->w () - 100, 25,
																	"Filename: ");
		export_filename->value (".");
		Fl_Button *button  = new Fl_Button (window->w() - 80, 180,
																				70, 25, "Save");
		button->callback (choose_export_cb);


		Fl_Button *back = new Fl_Button(180, 220, 100, 25, "@<- Back");
		back->callback(back_to_view_cb);

		Fl_Button *done = new Fl_Button (290, 220, 100, 25, "Finish");
		done->callback (done_cb);
		g->end ();
	}
	wizard->end ();
	window->end ();
	window->show (argc, argv);
	return Fl::run ();
}
