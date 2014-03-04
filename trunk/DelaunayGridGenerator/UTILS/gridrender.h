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

#include "piecewiselinearcomplex.h"

using namespace Wt;

namespace Utilities
{
    /// Ui web widget for WebGl render
    /// Algorithm: just see
    /// http://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WGLWidget.html
    class GridRender : public WGLWidget
    {       
        public : enum RENDERING_MODE {MODE_2D, MODE_3D};
        /// default MODE_3D, see constructor
        private: RENDERING_MODE _renderingMode;
        public : RENDERING_MODE getRenderingMode() const {return _renderingMode;}
        public : void setRenderingMode(RENDERING_MODE mode){_renderingMode = mode;}

        /// \todo not shure that this should be double, maybe float?
        private: std::vector<double>     _renderingNodesPositions;
        //private: std::vector<double>     _renderingNodesColors;
        //private: std::vector<double>     _renderingDataIndexes; /// \todo temporarily unused*/

        private: Program                _shaderProgram;

        private: AttribLocation         _attributeVertexPosition;
        private: UniformLocation        _uniformVertexColor;

        private: UniformLocation        _uniformModelMatrix;
        private: UniformLocation        _uniformViewMatrix;
        private: UniformLocation        _uniformClientMatrix;
        private: UniformLocation        _uniformProjectionMatrix;

        private: Buffer                 _renderingNodesVertexPositionBuffer;
        private: Buffer                 _renderingSegmentsIndexBuffer;

        private: Buffer                 _renderingFacetsVertexPositionBuffer;
        private: Buffer                 _renderingFacetsVertexColorBuffer;
        private: Buffer                 _renderingFacetsIndexBuffer;

        private: JavaScriptMatrix4x4    _jsMatrix;
        private: WMatrix4x4             _worldView;
        private: WMatrix4x4             _worldProjection;

        public: GridRender(WContainerWidget *parent);

        /// default [1.0, 1.0, 1.0, 1.0] - white, see constructor
        private: FEM::Vector4D _PlcNodesColor;
        public : void setRenderingPlcNodesColor(double R, double G, double B, double A)
        {
            _PlcNodesColor(R,G,B,A);
        }
        /// default [0.0, 1.0, 0.0, 1.0] - green, see constructor
        private: FEM::Vector4D _PlcSegmentsColor;
        public : void setRenderingPlcSegmentsColor(double R, double G, double B, double A)
        {
            _PlcSegmentsColor(R,G,B,A);
        }

        private: DelaunayGridGenerator::CommonPlc2D *_refToRenderingPlc2D = nullptr;
        public: void setRenderingPiecewiseLinearComplex(
                DelaunayGridGenerator::CommonPlc2D *renderingPlc2D) throw (std::logic_error);
        private: void _initializePlc2DNodesBuffers();
        private: void _initializePlc2DFacetsBuffers();
        private: void _drawPlc2DNodes();
        private: void _drawPlc2DFacets();

        private: DelaunayGridGenerator::CommonPlc3D *_refToRenderingPlc3D = nullptr;
        public: void setRenderingPiecewiseLinearComplex(
                DelaunayGridGenerator::CommonPlc3D *renderingPlc3D) throw (std::logic_error);

        /// See WGLWidget::bufferDatafv() and WGLWidget::renderfv() for more info
        private: void _initializePlc3DNodesBuffers() throw(std::runtime_error);
        private: void _initializePlc3DSegmentsBuffers() throw(std::runtime_error);
        private: void _initializePlc3DFacetsBuffers() throw(std::runtime_error);
        private: void _drawPlc3DNodes() throw(std::runtime_error);
        private: void _drawPlc3DSegments();
        private: void _drawPlc3DFacets();

        private: void _preloadAllBuffers();

        public: void initializeGL() override;
        public: void paintGL() override;
        public: void resizeGL(int width, int height) override;
        public: void updateGL() override;
    };

    /// See WGLWidget::makeFloat() -> Utils::round_js_str() -> round_js_str()
    /// at src/web/WebUtils.c
    /// \todo replace this function
    static inline char *generic_double_to_str(double d, char *buf)
    {
        if (!boost::math::isnan(d)) {
            if (!boost::math::isinf(d)) {
                sprintf(buf, "%.7e", d);
            } else {
                if (d > 0) {
                    sprintf(buf, "Infinity");
                } else {
                    sprintf(buf, "-Infinity");
                }
            }
        } else {
            sprintf(buf, "NaN");
        }
        return buf;
    }

    /*static inline char *itoa(int value, char *result, int base = 10) {
      char* out = result;
      int quotient = value;

      if (quotient < 0)
        quotient = -quotient;

      do {
        *out =
          "0123456789abcdefghijklmnopqrstuvwxyz"[quotient % base];
        ++out;
        quotient /= base;
      } while (quotient);

      if (value < 0 && base == 10)
        *out++ = '-';

      std::reverse(result, out);
      *out = 0;

      return result;
    }*/
}

#endif // GRIDRENDER_H
