#include "simpleglrender.h"

SimpleGLRender::SimpleGLRender(QWidget *pwgt) noexcept:
    QGLWidget(QGLFormat(QGL::SampleBuffers),pwgt),
    CameraPos(0.0, 0.0, 2.0),
    ScenePos(0.0, 0.0, 0.0),
    angleOY(0),
    angleOX(0),
    isPressed(false),
    dataString("Test string"),
    _PlcNodesColor(1.0, 1.0, 1.0, 1.0),
    _PlcSegmentsColor(0.0, 1.0, 0.0, 1.0),
    _PlcFacetsColor(0.8, 0.8, 0.8, 1.0)
{
}

void SimpleGLRender::initializeGL()
{
    this->qglClearColor(Qt::gray);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    QGLFormat aGLFormat;
    aGLFormat.setSampleBuffers(true);
    aGLFormat.setVersion(aGLFormat.majorVersion(),aGLFormat.minorVersion());
    QGLFormat::setDefaultFormat(aGLFormat);

    GLfloat light0_diffuse[] = {1.0, 1.0, 1.0};
    GLfloat light0_direction[] = {0.0, 0.0, 1.0, 0.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, light0_diffuse);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
}

void SimpleGLRender::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60.0f, ((GLfloat)nWidth)/(GLfloat)nHeight, 0.1, 1000.0f );
    glViewport(0,0,(GLint)nWidth,(GLint)nHeight);
}

void SimpleGLRender::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    _drawOrigin();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(CameraPos[0], CameraPos[1], CameraPos[2],
              0, 0, 0,
              0, 1, 0);
    glRotated(angleOY, 0, 1, 0);
    glRotated(angleOX, 1, 0, 0);
    glTranslated(-ScenePos[0], -ScenePos[1], -ScenePos[2]);

    _drawPlc3DFacets();
    _drawPlc3DSegments();
    _drawPlc3DNodes();

    _drawDelaunayGridGenerator3DNodes();
    _drawDelaunayGridGenerator3DFacets();
    _drawDelaunayGridGenerator3DElements();

    this->renderText(1.0, 10.0, dataString);
}

void SimpleGLRender::mouseMoveEvent(QMouseEvent *e)
{
    if(isPressed)
    {
        angleOY += e->x()-OldMouseX;
        angleOX += e->y()-OldMouseY;
        OldMouseX = e->x();
        OldMouseY = e->y();
        this->updateGL();
    }
}

void SimpleGLRender::mousePressEvent(QMouseEvent *e)
{
    isPressed = true;
    OldMouseX = e->x();
    OldMouseY = e->y();
    dataString =
            "Click! at(" +
            QString::number(OldMouseX)+ "," +
            QString::number(OldMouseY) + ")";
    this->updateGL();
}

void SimpleGLRender::mouseReleaseEvent(QMouseEvent *e)
{
    isPressed = false;
}

void SimpleGLRender::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space)
    {
        dataString = "Spacebar! New iteration";
        _ptrToRenderingDelaunayGridGenerator3D->_TEST_iteration(_ptrToRenderingPlc3D);
        this->updateGL();
    }
}

void SimpleGLRender::_drawPlc3DNodes() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc3D)
        throw std::runtime_error("_drawPlc3DNodes(),  bad pointer to PLC");

    if(_ptrToRenderingPlc3D->getNodeList().size() != 0)
    {
        glDisable(GL_LIGHTING);
        glPointSize(3);
        glColor4d(_PlcNodesColor[0], _PlcNodesColor[1], _PlcNodesColor[2], _PlcNodesColor[3]);

        //unsigned _n = 0;
        for(auto _i = _ptrToRenderingPlc3D->getNodeList().begin();
            _i != _ptrToRenderingPlc3D->getNodeList().end(); ++_i)
        {
            glBegin(GL_POINTS);
            glVertex3d((*_i)[0], (*_i)[1], (*_i)[2]);
            glEnd();
            //this->renderText((*_i)[0], (*_i)[1], (*_i)[2], QString::number(_n));
            //++_n;
        }
        glPointSize(1);
        glEnable(GL_LIGHTING);
    }
}

void SimpleGLRender::_drawPlc3DSegments() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc3D)
        throw std::runtime_error("_drawPlc3DNodes(),  bad pointer to PLC");

    if(_ptrToRenderingPlc3D->getSegmentList().size() != 0)
    {
        glColor4d(
                _PlcSegmentsColor[0],
                _PlcSegmentsColor[1],
                _PlcSegmentsColor[2],
                _PlcSegmentsColor[3]);
        for(auto _i = _ptrToRenderingPlc3D->getSegmentList().begin();
            _i != _ptrToRenderingPlc3D->getSegmentList().end(); ++_i)
        {
            glBegin(GL_LINES);
            glVertex3d((*_i)[0][0], (*_i)[0][1], (*_i)[0][2]);
            glVertex3d((*_i)[1][0], (*_i)[1][1], (*_i)[1][2]);
            glEnd();
        }
    }
}

void SimpleGLRender::_drawPlc3DFacets() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc3D)
        throw std::runtime_error("_drawPlc3DNodes(),  bad pointer to PLC");

    if(_ptrToRenderingPlc3D->getFacetList().size() != 0)
    {
        glColor4d(
                _PlcFacetsColor[0],
                _PlcFacetsColor[1],
                _PlcFacetsColor[2],
                _PlcFacetsColor[3]);
        for(auto _i = _ptrToRenderingPlc3D->getFacetList().begin();
            _i != _ptrToRenderingPlc3D->getFacetList().end(); ++_i)
        {

            glNormal3fv(((*_i)[1] - (*_i)[0]).crossProduct(
                        (*_i)[2] - (*_i)[0]).getCoordinates());
            glBegin(GL_TRIANGLES);
            glVertex3d((*_i)[0][0], (*_i)[0][1], (*_i)[0][2]);
            glVertex3d((*_i)[1][0], (*_i)[1][1], (*_i)[1][2]);
            glVertex3d((*_i)[2][0], (*_i)[2][1], (*_i)[2][2]);
            glEnd();
        }
    }
}

void SimpleGLRender::_drawDelaunayGridGenerator3DNodes() throw(std::runtime_error)
{
    if(!_ptrToRenderingDelaunayGridGenerator3D)
        throw std::runtime_error("_drawDelaunayGridGenerator3DNodes(),  bad pointer to Generator");

    if(_ptrToRenderingDelaunayGridGenerator3D->getNodeList().size() != 0)
    {
        unsigned _n = 0;
        for(auto _i = _ptrToRenderingDelaunayGridGenerator3D->getNodeList().begin();
            _i != _ptrToRenderingDelaunayGridGenerator3D->getNodeList().end(); ++_i)
        {
            glDisable(GL_LIGHTING);
            if((*_i).getState() == DelaunayGridGenerator::WrappedNode3D::STATE_ALIVE)
                glColor3ub(255,0,0);
            else
                glColor3ub(255,255,0);
            glBegin(GL_POINTS);
            glVertex3d((*_i)[0], (*_i)[1], (*_i)[2]);
            glEnd();
            this->renderText((*_i)[0], (*_i)[1], (*_i)[2], QString::number(_n));
            ++_n;
            glEnable(GL_LIGHTING);
        }
    }
}

void SimpleGLRender::_drawDelaunayGridGenerator3DFacets() throw(std::runtime_error)
{
    if(!_ptrToRenderingDelaunayGridGenerator3D)
        throw std::runtime_error("_drawDelaunayGridGenerator3DFacets(),  bad pointer to Generator");

    if(_ptrToRenderingDelaunayGridGenerator3D->getAliveFacetsList().size() != 0)
    {
        for(auto _i = _ptrToRenderingDelaunayGridGenerator3D->getAliveFacetsList().begin();
            _i != _ptrToRenderingDelaunayGridGenerator3D->getAliveFacetsList().end(); ++_i)
        {
            glColor3ub(200,200,200);
            glNormal3fv(((**_i)[1] - (**_i)[0]).crossProduct(
                        (**_i)[2] - (**_i)[0]).getCoordinates());
            glBegin(GL_TRIANGLES);
            glVertex3d((**_i)[0][0], (**_i)[0][1], (**_i)[0][2]);
            glVertex3d((**_i)[1][0], (**_i)[1][1], (**_i)[1][2]);
            glVertex3d((**_i)[2][0], (**_i)[2][1], (**_i)[2][2]);
            glEnd();

            glDisable(GL_LIGHTING);
            glColor3ub(255,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3d((**_i)[0][0], (**_i)[0][1], (**_i)[0][2]);
            glVertex3d((**_i)[1][0], (**_i)[1][1], (**_i)[1][2]);
            glVertex3d((**_i)[2][0], (**_i)[2][1], (**_i)[2][2]);
            glEnd();
            glEnable(GL_LIGHTING);
        }
    }

    if(_ptrToRenderingDelaunayGridGenerator3D->getDeadFacetsList().size() != 0)
    {
        glColor3ub(0,255,255);
        for(auto _i = _ptrToRenderingDelaunayGridGenerator3D->getDeadFacetsList().begin();
            _i != _ptrToRenderingDelaunayGridGenerator3D->getDeadFacetsList().end(); ++_i)
        {
            glDisable(GL_LIGHTING);
            glBegin(GL_LINE_LOOP);
            glVertex3d((**_i)[0][0], (**_i)[0][1], (**_i)[0][2]);
            glVertex3d((**_i)[1][0], (**_i)[1][1], (**_i)[1][2]);
            glVertex3d((**_i)[2][0], (**_i)[2][1], (**_i)[2][2]);
            glEnd();
            glEnable(GL_LIGHTING);
        }
    }
}

void SimpleGLRender::_drawDelaunayGridGenerator3DElements() throw(std::runtime_error)
{

}

void SimpleGLRender::_drawOrigin() noexcept
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

        glLoadIdentity();
        gluPerspective( 90.0f, 1, 0.1, 1000.0f );
        int _width = this->width();
        int _height = this->height();
        glViewport(0,0,100,100);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

            glLoadIdentity();
            gluLookAt(0, 0, 2,
                      0, 0, 0,
                      0, 1, 0);
            glRotated(angleOY, 0, 1, 0);
            glRotated(angleOX, 1, 0, 0);

            glLineWidth(3);

            glBegin(GL_LINES);
            glColor3ub(255, 0, 0);
            glVertex3d(0, 0, 0);
            glVertex3d(1, 0, 0);
            glEnd();
            this->renderText(1, 0, 0, "x");

            glBegin(GL_LINES);
            glColor3ub(0, 255, 0);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 1, 0);
            glEnd();
            this->renderText(0, 1, 0, "y");

            glBegin(GL_LINES);
            glColor3ub(0, 0, 255);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 0, 1);
            glEnd();
            this->renderText(0, 0, 1, "z");

            glLineWidth(1);

        glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glViewport(0,0,_width,_height);
    glEnable(GL_LIGHTING);
}
