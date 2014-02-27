#ifndef GRIDRENDER_H
#define GRIDRENDER_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include "Wt/WGLWidget"
#include "Wt/WMatrix4x4"

using namespace Wt;

namespace Utilities
{
    class GridRender : public WGLWidget
    {
        private: Program                _shaderProgram;
        private: AttribLocation         _vertexPositionAttribute;
        private: AttribLocation         _vertexColorAttribute;
        private: UniformLocation        _uniformModelMatrix;
        private: UniformLocation        _uniformViewMatrix;
        private: UniformLocation        _uniformClientMatrix;
        private: UniformLocation        _uniformProjectionMatrix;
        private: Buffer                 _renderingObjectVertexPositionBuffer;
        private: Buffer                 _renderingObjectVertexColorBuffer;
        private: Buffer                 _renderingObjectElementBuffer;

        private: JavaScriptMatrix4x4    _jsMatrix;
        private: WMatrix4x4             _worldView;
        private: WMatrix4x4             _worldProjection;

        public: GridRender(WContainerWidget *parent);
        public: void initializeGL() override;
        public: void paintGL() override;
        public: void resizeGL(int width, int height) override;
    };
}

#endif // GRIDRENDER_H
