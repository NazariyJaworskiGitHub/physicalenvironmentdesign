#include "simpleglrender2d.h"

#include "iostream"

SimpleGLRender2D::SimpleGLRender2D(QWidget *pwgt) noexcept :
    SimpleGLRender(pwgt)
{
}

void SimpleGLRender2D::_drawOrigin() noexcept
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

        glLoadIdentity();
        gluPerspective( 60.0f, 1.0, 0.1, 1000.0f );
        int _width = this->width();
        int _height = this->height();
        glViewport(0,0,100,100);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

            glLoadIdentity();
            gluLookAt(0, 0, 2,
                      0, 0, 0,
                      0, 1, 0);
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
            glLineWidth(1);

        glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glViewport(0,0,_width,_height);
    glEnable(GL_LIGHTING);
}

void SimpleGLRender2D::_RotateAndTranslate() noexcept
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(_CameraPosition[0], _CameraPosition[1], _CameraPosition[2],
              0, 0, 0,
              0, 1, 0);
    glScaled(_Zoom, _Zoom, _Zoom);
    glTranslated(-_ScenePosition[0], -_ScenePosition[1], -_ScenePosition[2]);
}

void SimpleGLRender2D::mouseMoveEvent(QMouseEvent *e)
{
    if(_isPressed)
    {
        _ScenePosition[0] += -double(e->x()-_oldMouseX) / this->width() * 2.0 / _Zoom;
        _ScenePosition[1] += double(e->y()-_oldMouseY) / this->width()  * 2.0 / _Zoom;
        _oldMouseX = e->x();
        _oldMouseY = e->y();
        this->updateGL();
    }
}

void SimpleGLRender2D::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space)
    {
        _ptrToRenderingDelaunayGridGenerator2D->_TEST_iteration(_ptrToRenderingPlc2D);
        dataString = QString(" AN: ") +
                QString::number(_ptrToRenderingDelaunayGridGenerator2D->getAliveNodeList().size()) +
                QString(" DN: ") +
                QString::number(_ptrToRenderingDelaunayGridGenerator2D->getDeadNodeList().size());
        this->updateGL();
    }
    /// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    else if(e->key() == Qt::Key_I)
    {
        std::cout << *_ptrToRenderingDelaunayGridGenerator2D;
    }
}

void SimpleGLRender2D::_drawPlcNodes() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc2D)
        throw std::runtime_error("_drawPlcNodes(),  bad pointer to PLC");

    if(_ptrToRenderingPlc2D->getNodeList().size() != 0)
    {
        glDisable(GL_LIGHTING);
        glPointSize(2);
        glColor4d(_PlcNodesColor[0], _PlcNodesColor[1], _PlcNodesColor[2], _PlcNodesColor[3]);
        glBegin(GL_POINTS);
        for(auto _i = _ptrToRenderingPlc2D->getNodeList().begin();
            _i != _ptrToRenderingPlc2D->getNodeList().end(); ++_i)
            glVertex2d((*_i)[0], (*_i)[1]);
        glEnd();
        glPointSize(1);
        glEnable(GL_LIGHTING);
    }
}

void SimpleGLRender2D::_drawPlcSegments() throw(std::runtime_error)
{
}

void SimpleGLRender2D::_drawPlcFacets() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc2D)
        throw std::runtime_error("_drawPlcFacets(),  bad pointer to PLC");

    if(_ptrToRenderingPlc2D->getFacetList().size() != 0)
    {
        glColor4d(
                _PlcFacetsColor[0],
                _PlcFacetsColor[1],
                _PlcFacetsColor[2],
                _PlcFacetsColor[3]);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        for(auto _i = _ptrToRenderingPlc2D->getFacetList().begin();
            _i != _ptrToRenderingPlc2D->getFacetList().end(); ++_i)
        {
            glVertex2d((*_i)[0][0], (*_i)[0][1]);
            glVertex2d((*_i)[1][0], (*_i)[1][1]);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

void SimpleGLRender2D::_drawDelaunayGridGeneratorNodes() throw(std::runtime_error)
{
    if(!_ptrToRenderingDelaunayGridGenerator2D)
        throw std::runtime_error("_drawDelaunayGridGeneratorNodes(),  bad pointer to Generator");

    if(_ptrToRenderingDelaunayGridGenerator2D->getNodeList().size() != 0)
    {
        glDisable(GL_LIGHTING);
        glPointSize(3);
        glBegin(GL_POINTS);
        for(auto _i = _ptrToRenderingDelaunayGridGenerator2D->getNodeList().begin();
            _i != _ptrToRenderingDelaunayGridGenerator2D->getNodeList().end(); ++_i)
        {
            if((*_i).getState() == DelaunayGridGenerator::WrappedNode2D::STATE_ALIVE)
                glColor4d(
                        _GeneratorAliveNodesColor[0],
                        _GeneratorAliveNodesColor[1],
                        _GeneratorAliveNodesColor[2],
                        _GeneratorAliveNodesColor[3]);
            else
                glColor4d(
                        _GeneratorDeadNodesColor[0],
                        _GeneratorDeadNodesColor[1],
                        _GeneratorDeadNodesColor[2],
                        _GeneratorDeadNodesColor[3]);
            glVertex2d((*_i)[0], (*_i)[1]);
        }
        glEnd();
        glPointSize(1);

        glColor4d(
                _TextColor[0],
                _TextColor[1],
                _TextColor[2],
                _TextColor[3]);
        unsigned _n = 0;
        for(auto _i = _ptrToRenderingDelaunayGridGenerator2D->getNodeList().begin();
            _i != _ptrToRenderingDelaunayGridGenerator2D->getNodeList().end(); ++_i)
        {
            this->renderText((*_i)[0], (*_i)[1], 0.0, QString::number(_n));
            ++_n;
        }

        glEnable(GL_LIGHTING);
    }
}

void SimpleGLRender2D::_drawDelaunayGridGeneratorFacets() throw(std::runtime_error)
{
    if(!_ptrToRenderingDelaunayGridGenerator2D)
        throw std::runtime_error("_drawDelaunayGridGeneratorFacets(),  bad pointer to Generator");

    glDisable(GL_LIGHTING);
    if(_ptrToRenderingDelaunayGridGenerator2D->getAliveFacetsList().size() != 0)
    {
        glBegin(GL_LINES);
        glColor4d(
                _GeneratorTopAliveFacetColor[0],
                _GeneratorTopAliveFacetColor[1],
                _GeneratorTopAliveFacetColor[2],
                _GeneratorTopAliveFacetColor[3]);
        // First facet
        auto _i = _ptrToRenderingDelaunayGridGenerator2D->getAliveFacetsList().begin();
        glLineWidth(2);
        glVertex2d((**_i)[0][0], (**_i)[0][1]);
        glVertex2d((**_i)[1][0], (**_i)[1][1]);
        glLineWidth(1);
        ++_i;
        // Resr facets
        glColor4d(
                _GeneratorAliveFacetsColor[0],
                _GeneratorAliveFacetsColor[1],
                _GeneratorAliveFacetsColor[2],
                _GeneratorAliveFacetsColor[3]);
        for(;
            _i != _ptrToRenderingDelaunayGridGenerator2D->getAliveFacetsList().end(); ++_i)
        {
            glVertex2d((**_i)[0][0], (**_i)[0][1]);
            glVertex2d((**_i)[1][0], (**_i)[1][1]);
        }
        glEnd();
    }

    if(_ptrToRenderingDelaunayGridGenerator2D->getDeadFacetsList().size() != 0)
    {

        glBegin(GL_LINES);
        for(auto _i = _ptrToRenderingDelaunayGridGenerator2D->getDeadFacetsList().begin();
            _i != _ptrToRenderingDelaunayGridGenerator2D->getDeadFacetsList().end(); ++_i)
        {
            if((**_i).isMetastructure())
                glColor4d(
                        _GeneratorMetastructureFacetColor[0],
                        _GeneratorMetastructureFacetColor[1],
                        _GeneratorMetastructureFacetColor[2],
                        _GeneratorMetastructureFacetColor[3]);
            else
                glColor4d(
                        _GeneratorDeadFacetsColor[0],
                        _GeneratorDeadFacetsColor[1],
                        _GeneratorDeadFacetsColor[2],
                        _GeneratorDeadFacetsColor[3]);
            glVertex2d((**_i)[0][0], (**_i)[0][1]);
            glVertex2d((**_i)[1][0], (**_i)[1][1]);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
}
