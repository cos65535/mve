#ifndef VIEW_INSPECT_HEADER
#define VIEW_INSPECT_HEADER

#include <string>
#include <QtGui>

#include "mve/view.h"

#include "scrollimage.h"
#include "transferfunction.h"
#include "imageinspector.h"
#include "imageoperations.h"

class ViewInspect : public QWidget
{
    Q_OBJECT

private:
    ScrollImage* scroll_image;
    QComboBox* embeddings;
    QToolBar* toolbar;
    QTabWidget* image_details;

    QLabel* label_name;
    QLabel* label_dimension;
    QLabel* label_memory;

    TransferFunctionWidget* tfunc;
    TransferFunction func;
    ImageInspectorWidget* inspector;
    ImageOperationsWidget* operations;
    QTextEdit* exif_viewer;

    //mve::Scene::Ptr scene;
    mve::View::Ptr view;
    mve::ImageBase::Ptr image;
    std::string recent_embedding;

    QAction* action_open;
    QAction* action_reload;
    QAction* action_save_view;
    QAction* action_export_ply;
    QAction* action_export_image;
    QAction* action_zoom_in;
    QAction* action_zoom_out;
    QAction* action_zoom_reset;
    QAction* action_zoom_fit;
    QAction* action_show_details;
    QAction* action_copy_embedding;
    QAction* action_del_embedding;

private slots:
    void on_open (void);
    void on_zoom_in (void);
    void on_zoom_out (void);
    void on_normal_size (void);
    void on_fit_to_window (void);
    void on_details_toggled (void);
    void on_embedding_selected (QString const& name);
    void on_ply_export (void);
    void on_image_export (void);
    void on_copy_embedding (void);
    void on_del_embedding (void);
    void on_save_view (void);
    void on_view_reload (void);
    void on_reload_embeddings (void);
    void on_image_clicked (int x, int y, QMouseEvent* event);
    void on_tf_changed (TransferFunction func);

    void on_scene_selected (mve::Scene::Ptr scene);
    void on_view_selected (mve::View::Ptr view);

private:
    void create_detail_frame (void);
    void create_actions (void);
    void create_menus (void);
    void update_actions (void);
    void load_recent_embedding (void);
    void populate_embeddings (void);
    void populate_exif_viewer (void);
    void set_embedding (std::string const& name);

public:
    ViewInspect (QWidget* parent = 0);

    void display_image (mve::ByteImage::Ptr img);
    void display_image (mve::FloatImage::Ptr img);

    void show_details (bool show);

    void load_file (QString filename);
    void load_image_file (QString filename);
    void load_mve_file (QString filename);
    void load_ply_file (QString filename);

    void reset (void);
};

#endif /* VIEW_INSPECT_HEADER */
