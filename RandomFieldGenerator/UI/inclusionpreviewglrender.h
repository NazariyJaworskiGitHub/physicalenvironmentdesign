#ifndef INCLUSIONPREVIEWGLRENDER_H
#define INCLUSIONPREVIEWGLRENDER_H

#include "UI/volumeglrenderbasecontroller.h"
#include "representativevolumeelement.h"

namespace UserInterface
{
    /// \todo refactoring, see FilterPreviewGLRender
    class InclusionPreviewGLRender : public VolumeGLRenderBaseController
    {
        private: const RepresentativeVolumeElement* _ptrToRVE;

        /// \todo need refactoring, make some texture manager
        private: GLuint _displayListBaseID; // just increment for next list id
        private: GLuint _textureIDs[1];     // it is array because unified approach to texturing
        public : void loadDataIntoTexture() throw(std::runtime_error);
        private: void _prepareDataTextureDisplayList() noexcept;

        public : virtual void initializeGLEW() throw(std::runtime_error);
        public : virtual void paintGL() override;

        public: InclusionPreviewGLRender(
                const RepresentativeVolumeElement *RVE,
                QWidget *pwgt) noexcept;
        public: ~InclusionPreviewGLRender();
    };
}

#endif // INCLUSIONPREVIEWGLRENDER_H
