#include "simpleglrender3d.h"

#include <iostream>

SimpleGLRender3D::SimpleGLRender3D(QWidget *pwgt) noexcept :
    SimpleGLRender(pwgt)
{
}

void SimpleGLRender3D::_drawOrigin() noexcept
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

void SimpleGLRender3D::_RotateAndTranslate() noexcept
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(_CameraPosition[0], _CameraPosition[1], _CameraPosition[2],
              0, 0, 0,
              0, 1, 0);
    glRotated(angleOY, 0, 1, 0);
    glRotated(angleOX, 1, 0, 0);
    glScaled(_Zoom, _Zoom, _Zoom);
    glTranslated(-_ScenePosition[0], -_ScenePosition[1], -_ScenePosition[2]);
}

void SimpleGLRender3D::mouseMoveEvent(QMouseEvent *e)
{
    if(_isPressed)
    {
        angleOY += e->x()-_oldMouseX;
        angleOX += e->y()-_oldMouseY;
        _oldMouseX = e->x();
        _oldMouseY = e->y();
        this->updateGL();
    }
}

void SimpleGLRender3D::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space)
    {
        _ptrToRenderingDelaunayGridGenerator3D->_TEST_iteration(_ptrToRenderingPlc3D);
        dataString = QString(" AN: ") +
                QString::number(_ptrToRenderingDelaunayGridGenerator3D->getAliveNodeList().size()) +
                QString(" DN: ") +
                QString::number(_ptrToRenderingDelaunayGridGenerator3D->getDeadNodeList().size())+
                QString(" Iteration: ") +
                QString::number(_ptrToRenderingDelaunayGridGenerator3D->_iteration);
        this->updateGL();

        /// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if(!_ptrToRenderingDelaunayGridGenerator3D->getElementsList().empty())
        {
        std::cout
                << _ptrToRenderingDelaunayGridGenerator3D->_iteration << ": "
                << _ptrToRenderingDelaunayGridGenerator3D->getElementsList().back()->getNodeIndexes()[0] << " "
                << _ptrToRenderingDelaunayGridGenerator3D->getElementsList().back()->getNodeIndexes()[1] << " "
                << _ptrToRenderingDelaunayGridGenerator3D->getElementsList().back()->getNodeIndexes()[2] << " "
                << _ptrToRenderingDelaunayGridGenerator3D->getElementsList().back()->getNodeIndexes()[3] << "\n";
        }
    }
    /// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    else if(e->key() == Qt::Key_I)
    {
        std::cout << *_ptrToRenderingDelaunayGridGenerator3D;
    }
    /// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    else if(e->key() == Qt::Key_Z && e->modifiers() == Qt::ControlModifier)
    {
        _ptrToRenderingDelaunayGridGenerator3D->_TEST_undo_iteration();
        this->updateGL();
    }
}

void SimpleGLRender3D::_drawPlcNodes() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc3D)
        throw std::runtime_error("_drawPlcNodes(),  bad pointer to PLC");

    if(_ptrToRenderingPlc3D->getNodeList().size() != 0)
    {
        glDisable(GL_LIGHTING);
        glPointSize(2);
        glColor4d(_PlcNodesColor[0], _PlcNodesColor[1], _PlcNodesColor[2], _PlcNodesColor[3]);
        glBegin(GL_POINTS);
        for(auto _i : _ptrToRenderingPlc3D->getNodeList())
            glVertex3d((*_i)[0], (*_i)[1], (*_i)[2]);
        glEnd();
        glPointSize(1);
        glEnable(GL_LIGHTING);
    }
}

void SimpleGLRender3D::_drawPlcSegments() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc3D)
        throw std::runtime_error("_drawPlcSegments(),  bad pointer to PLC");

    if(_ptrToRenderingPlc3D->getSegmentList().size() != 0)
    {
        glColor4d(
                _PlcSegmentsColor[0],
                _PlcSegmentsColor[1],
                _PlcSegmentsColor[2],
                _PlcSegmentsColor[3]);
        glBegin(GL_LINES);
        for(auto _i : _ptrToRenderingPlc3D->getSegmentList())
        {
            glVertex3d((*_i)[0][0], (*_i)[0][1], (*_i)[0][2]);
            glVertex3d((*_i)[1][0], (*_i)[1][1], (*_i)[1][2]);
        }
        glEnd();
    }
}

void SimpleGLRender3D::_drawPlcFacets() throw(std::runtime_error)
{
    if(!_ptrToRenderingPlc3D)
        throw std::runtime_error("_drawPlcFacets(),  bad pointer to PLC");

    if(_ptrToRenderingPlc3D->getFacetList().size() != 0)
    {
        glColor4d(
                _PlcFacetsColor[0],
                _PlcFacetsColor[1],
                _PlcFacetsColor[2],
                _PlcFacetsColor[3]);
        glBegin(GL_TRIANGLES);
        for(auto _i : _ptrToRenderingPlc3D->getFacetList())
        {
            glNormal3fv(((*_i)[1] - (*_i)[0]).crossProduct(
                        (*_i)[2] - (*_i)[0]).getCoordinates());

            glVertex3d((*_i)[0][0], (*_i)[0][1], (*_i)[0][2]);
            glVertex3d((*_i)[1][0], (*_i)[1][1], (*_i)[1][2]);
            glVertex3d((*_i)[2][0], (*_i)[2][1], (*_i)[2][2]);
        }
        glEnd();
    }
}

void SimpleGLRender3D::_drawDelaunayGridGeneratorNodes() throw(std::runtime_error)
{
    if(!_ptrToRenderingDelaunayGridGenerator3D)
        throw std::runtime_error("_drawDelaunayGridGeneratorNodes(),  bad pointer to Generator");

    if(_ptrToRenderingDelaunayGridGenerator3D->getNodeList().size() != 0)
    {
        glDisable(GL_LIGHTING);
        glPointSize(3);
        glBegin(GL_POINTS);
        for(auto _i : _ptrToRenderingDelaunayGridGenerator3D->getNodeList())
        {
            if((*_i).getState() == DelaunayGridGenerator::WrappedNode3D::STATE_ALIVE)
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
            glVertex3d((*_i)[0], (*_i)[1], (*_i)[2]);
        }
        glEnd();
        glPointSize(1);

        glColor4d(
                _TextColor[0],
                _TextColor[1],
                _TextColor[2],
                _TextColor[3]);
        unsigned _n = 0;
        for(auto _i : _ptrToRenderingDelaunayGridGenerator3D->getNodeList())
        {
            this->renderText((*_i)[0], (*_i)[1], (*_i)[2], QString::number(_n));
            ++_n;
        }

        glEnable(GL_LIGHTING);
    }
}

void SimpleGLRender3D::_drawDelaunayGridGeneratorFacets() throw(std::runtime_error)
{
    if(!_ptrToRenderingDelaunayGridGenerator3D)
        throw std::runtime_error("_drawDelaunayGridGeneratorFacets(),  bad pointer to Generator");

    if(_ptrToRenderingDelaunayGridGenerator3D->getDeadFacetsList().size() != 0)
    {
        glColor4d(
                _GeneratorMetastructureFacetColor[0],
                _GeneratorMetastructureFacetColor[1],
                _GeneratorMetastructureFacetColor[2],
                _GeneratorMetastructureFacetColor[3]);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBegin(GL_TRIANGLES);
        for(auto _i : _ptrToRenderingDelaunayGridGenerator3D->getDeadFacetsList())
        {
            if((*_i).isMetastructure())
            {
                glNormal3fv(((*_i)[1] - (*_i)[0]).crossProduct(
                            (*_i)[2] - (*_i)[0]).getCoordinates());
                glVertex3d((*_i)[0][0], (*_i)[0][1], (*_i)[0][2]);
                glVertex3d((*_i)[1][0], (*_i)[1][1], (*_i)[1][2]);
                glVertex3d((*_i)[2][0], (*_i)[2][1], (*_i)[2][2]);
            }
        }
        glEnd();
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glColor4d(
                _GeneratorDeadFacetsEdgesColor[0],
                _GeneratorDeadFacetsEdgesColor[1],
                _GeneratorDeadFacetsEdgesColor[2],
                _GeneratorDeadFacetsEdgesColor[3]);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        for(auto _i : _ptrToRenderingDelaunayGridGenerator3D->getDeadFacetsList())
        {
            glVertex3d((*_i)[0][0], (*_i)[0][1], (*_i)[0][2]);
            glVertex3d((*_i)[1][0], (*_i)[1][1], (*_i)[1][2]);

            glVertex3d((*_i)[1][0], (*_i)[1][1], (*_i)[1][2]);
            glVertex3d((*_i)[2][0], (*_i)[2][1], (*_i)[2][2]);

            glVertex3d((*_i)[2][0], (*_i)[2][1], (*_i)[2][2]);
            glVertex3d((*_i)[0][0], (*_i)[0][1], (*_i)[0][2]);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    if(_ptrToRenderingDelaunayGridGenerator3D->getAliveFacetsList().size() != 0)
    {
        glBegin(GL_TRIANGLES);
        glColor4d(
                _GeneratorTopAliveFacetColor[0],
                _GeneratorTopAliveFacetColor[1],
                _GeneratorTopAliveFacetColor[2],
                _GeneratorTopAliveFacetColor[3]);
        // First facet
        auto _i = _ptrToRenderingDelaunayGridGenerator3D->getAliveFacetsList().begin();
        glNormal3fv(((**_i)[1] - (**_i)[0]).crossProduct(
                    (**_i)[2] - (**_i)[0]).getCoordinates());
        glVertex3d((**_i)[0][0], (**_i)[0][1], (**_i)[0][2]);
        glVertex3d((**_i)[1][0], (**_i)[1][1], (**_i)[1][2]);
        glVertex3d((**_i)[2][0], (**_i)[2][1], (**_i)[2][2]);
        ++_i;
        // Resr facets
        glColor4d(
                _GeneratorAliveFacetsColor[0],
                _GeneratorAliveFacetsColor[1],
                _GeneratorAliveFacetsColor[2],
                _GeneratorAliveFacetsColor[3]);
        for(;
            _i != _ptrToRenderingDelaunayGridGenerator3D->getAliveFacetsList().end(); ++_i)
        {
            glNormal3fv(((**_i)[1] - (**_i)[0]).crossProduct(
                        (**_i)[2] - (**_i)[0]).getCoordinates());
            glVertex3d((**_i)[0][0], (**_i)[0][1], (**_i)[0][2]);
            glVertex3d((**_i)[1][0], (**_i)[1][1], (**_i)[1][2]);
            glVertex3d((**_i)[2][0], (**_i)[2][1], (**_i)[2][2]);
        }
        glEnd();
        glDisable(GL_LIGHTING);
    }
}
