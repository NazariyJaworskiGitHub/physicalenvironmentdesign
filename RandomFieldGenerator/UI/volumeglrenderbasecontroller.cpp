#include "volumeglrenderbasecontroller.h"

#include <iostream>
#include <sstream>

#include "constants.h"

using namespace UserInterface;

VolumeGLRenderBaseController::VolumeGLRenderBaseController(QWidget *pwgt) noexcept:
    QGLWidget(QGLFormat(QGL::SampleBuffers),pwgt),
    _contextMenu(new QMenu(this)),
    _actionFormat(new QAction("Format...", this))
{
    connect(_actionFormat, SIGNAL(triggered()), this, SLOT(slot_createFormatDialog()));
    _contextMenu->addAction(_actionFormat);
    _contextMenu->addSeparator();

    initializeGL();
}

void VolumeGLRenderBaseController::mousePressEvent(QMouseEvent *e)
{
    _isPressed = true;
    _oldMouseX = e->x();
    _oldMouseY = e->y();
    this->updateGL();
}

void VolumeGLRenderBaseController::mouseReleaseEvent(QMouseEvent *e)
{
    _isPressed = false;

    if(e->button() == Qt::RightButton)
    {
        _contextMenu->exec(mapToGlobal(e->pos()));
    }
//    QGLWidget::mouseReleaseEvent(event);  //Dont forget to pass on the event to parent
}

void VolumeGLRenderBaseController::mouseMoveEvent(QMouseEvent *e)
{
    if(_isPressed)
    {
        QMatrix4x4 _m;
        _m.setToIdentity();
        _m.rotate( (e->x()-_oldMouseX), 0, 1, 0);
        _m.rotate( (e->y()-_oldMouseY), 1, 0, 0);
        _mControl = _m * _mControl;
        _oldMouseX = e->x();
        _oldMouseY = e->y();
        this->updateGL();
    }
}

void VolumeGLRenderBaseController::wheelEvent(QWheelEvent *e)
{
    _Zoom += _Zoom * e->delta() / 5000;
    this->updateGL();
}

void VolumeGLRenderBaseController::keyPressEvent(QKeyEvent *e)
{
    //////////////////////////////////////////////////////////////////////////
}

void VolumeGLRenderBaseController::_renderMultilineText(
        int x, int y, const QString &text, const QFont &f) noexcept
{
    QStringList _lines = text.split('\n', QString::SkipEmptyParts);
    glColor4f(
            _TextColor.redF(),
            _TextColor.greenF(),
            _TextColor.blueF(),
            _TextColor.alphaF());
    int _lineHeight = 2 + QFontInfo(f).pixelSize();
    for(auto _str: _lines)
    {
        y += _lineHeight;
        this->renderText(x, y, _str, f);
    }
}

void VolumeGLRenderBaseController::_drawArrow(
        GLfloat x1, GLfloat y1, GLfloat z1,
        GLfloat x2, GLfloat y2, GLfloat z2) noexcept
{
    float x = x2 - x1;
    float y = y2 - y1;
    float z = z2 - z1;
    float L = std::sqrt(x*x + y*y + z*z);
    float rBase = L * 0.05f;
    float rHat = L * 0.1f;
    float hatBegin = L * 0.75f;

    GLUquadricObj *quadObj;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix ();
    {
        // Rotate and translate
        glTranslated(x1,y1,z1);
        if((x != 0.0f) || (y != 0.0f))
        {
            glRotated(std::atan2(y, x) * 180 / M_PI, 0.0f, 0.0f, 1.0f);
            glRotated(std::atan2(std::sqrt(x*x + y*y), z) * 180 / M_PI, 0.0f, 1.0f, 0.0f);
        }
        else if(z < 0.0f)
            glRotated(180.0f, 1.0f, 0.0f, 0.0f);

        // Base
        glTranslatef(0, 0, 0);
        quadObj = gluNewQuadric();
        gluQuadricDrawStyle(quadObj, GLU_FILL);
        gluQuadricNormals(quadObj, GLU_SMOOTH);
        gluCylinder(quadObj, rBase, rBase, hatBegin, 32, 1);
        gluDeleteQuadric(quadObj);

        quadObj = gluNewQuadric();
        gluQuadricDrawStyle(quadObj, GLU_FILL);
        gluQuadricNormals(quadObj, GLU_SMOOTH);
        gluDisk(quadObj, 0.0, rBase, 32, 1);
        gluDeleteQuadric(quadObj);

        // Hat
        glTranslatef(0.0f, 0.0f, hatBegin);
        quadObj = gluNewQuadric();
        gluQuadricDrawStyle(quadObj, GLU_FILL);
        gluQuadricNormals(quadObj, GLU_SMOOTH);
        gluCylinder(quadObj, rHat, 0.0, L - hatBegin, 32, 1);
        gluDeleteQuadric(quadObj);

        quadObj = gluNewQuadric();
        gluQuadricDrawStyle(quadObj, GLU_FILL);
        gluQuadricNormals(quadObj, GLU_SMOOTH);
        gluDisk(quadObj, 0.0, rBase, 32, 1);
        gluDeleteQuadric(quadObj);
    }
    glPopMatrix ();
}


/// \warning Intel old video driver bug, when using glColor3ub(255, 0, 0);
/// see https://bugreports.qt.io/browse/QTBUG-6217
/// keep your video driver up to date!
void VolumeGLRenderBaseController::_drawOrigin() noexcept
{
    int _width = this->width();
    int _height = this->height();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
        glLoadIdentity();
        gluPerspective( 45.0f, 4.0f/3.0f, 0.01f, 1000.0f );

        glViewport(0,0,160,120);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
            glLoadMatrixf((_mWorld * _mControl).constData());

            glColor3d(1, 0, 0);
            this->renderText(0.75f, 0, 0, "x", _TextFont);
            glColor3d(0, 1, 0);
            this->renderText(0, 0.75f, 0, "y", _TextFont);
            glColor3d(0, 0, 1);
            this->renderText(0, 0, 0.75f, "z", _TextFont);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);

            glColor3d(1, 0, 0);
            _drawArrow(0, 0, 0,
                       0.75f, 0, 0);

            glColor3d(0, 1, 0);
            _drawArrow(0, 0, 0,
                       0, 0.75f, 0);

            glColor3d(0, 0, 1);
            _drawArrow(0, 0, 0,
                       0, 0, 0.75f);

            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
        }
        glPopMatrix();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glViewport(0,0,_width,_height);
}

std::string VolumeGLRenderBaseController::printOpenGLInfo() const noexcept
{
    this->context()->makeCurrent();
    std::stringstream _str;
    _str
            << "OpenGL vendor  " << glGetString(GL_VENDOR) << std::endl
            << "       render  " << glGetString(GL_RENDERER) << std::endl
            << "       version " << glGetString(GL_VERSION) << std::endl
            << "       shaders " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl
            << "       ext.    " << glGetString(GL_EXTENSIONS) << std::endl;
    return _str.str();
}

void VolumeGLRenderBaseController::initializeGL()
{
    QGLFormat aGLFormat;
    aGLFormat.setSampleBuffers(true);
    aGLFormat.setVersion(aGLFormat.majorVersion(),aGLFormat.minorVersion());
    QGLFormat::setDefaultFormat(aGLFormat);

    glClearDepth(1.0f);
//    glEnable(GL_DEPTH_TEST);
//    //glEnable(GL_LINE_SMOOTH);
    glDepthFunc(GL_LEQUAL);
//    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
//    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

//    glEnable(GL_CULL_FACE);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLfloat light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light0_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // See position at paintGL()
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0f);
    glEnable(GL_LIGHT0);
//    glEnable(GL_LIGHTING);

    _mControl.setToIdentity();
    _mWorld.setToIdentity();
    _mWorld.lookAt(QVector3D(_CameraPosition[0],_CameraPosition[1],_CameraPosition[2]),
            QVector3D(0,0,0),QVector3D(0,1,0));
}

void VolumeGLRenderBaseController::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, ((GLfloat)nWidth)/(GLfloat)nHeight, 0.01f, 1000.0f );
    glViewport(0,0,(GLint)nWidth,(GLint)nHeight);
}

void VolumeGLRenderBaseController::_drawBoundingBox() noexcept
{
    glEnable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    {
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);  // x
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);  // y
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);  // z
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);

        glColor4f(0.75f, 0.75f, 0.75f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);

        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
    }
    glEnd();
    glDisable(GL_DEPTH_TEST);

    glColor4f(
            _TextColor.redF(),
            _TextColor.greenF(),
            _TextColor.blueF(),
            _TextColor.alphaF());

    this->renderText(  0.5f, -0.03f, -0.03f,
                       QString::number(_boundingBoxRepresentationSize) + "m", _TextFont);
    this->renderText(-0.03f,   0.5f, -0.03f,
                     QString::number(_boundingBoxRepresentationSize) + "m", _TextFont);
    this->renderText(-0.03f, -0.03f,   0.5f,
                     QString::number(_boundingBoxRepresentationSize) + "m", _TextFont);
}

VolumeGLRenderBaseController::~VolumeGLRenderBaseController()
{

}

void VolumeGLRenderBaseController::slot_createFormatDialog()
{
    UserInterface::VolumeGLRenderFormatDialog *_dialog =
            new UserInterface::VolumeGLRenderFormatDialog(this);
    _dialog->show();
}
