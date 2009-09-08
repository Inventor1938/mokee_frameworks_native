/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_LAYER_H
#define ANDROID_LAYER_H

#include <stdint.h>
#include <sys/types.h>

#include <ui/PixelFormat.h>
#include <pixelflinger/pixelflinger.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include "Buffer.h"
#include "LayerBase.h"
#include "Transform.h"

namespace android {

// ---------------------------------------------------------------------------

class Client;
class FreezeLock;
class Buffer;

// ---------------------------------------------------------------------------

const size_t NUM_BUFFERS = 2;

class Layer : public LayerBaseClient
{
public:    
    static const uint32_t typeInfo;
    static const char* const typeID;
    virtual char const* getTypeID() const { return typeID; }
    virtual uint32_t getTypeInfo() const { return typeInfo; }

    
    SharedBufferServer*     lcblk;

    
                 Layer(SurfaceFlinger* flinger, DisplayID display,
                         const sp<Client>& client, int32_t i);

        virtual ~Layer();

    status_t setBuffers(uint32_t w, uint32_t h, 
            PixelFormat format, uint32_t flags=0);

    void setDrawingSize(uint32_t w, uint32_t h);

    virtual void onDraw(const Region& clip) const;
    virtual uint32_t doTransaction(uint32_t transactionFlags);
    virtual void lockPageFlip(bool& recomputeVisibleRegions);
    virtual void unlockPageFlip(const Transform& planeTransform, Region& outDirtyRegion);
    virtual void finishPageFlip();
    virtual bool needsBlending() const      { return mNeedsBlending; }
    virtual bool isSecure() const           { return mSecure; }
    virtual sp<Surface> createSurface() const;
    virtual status_t ditch();
    
    // only for debugging
    inline sp<Buffer> getBuffer(int i) { return mBuffers[i]; }
    // only for debugging
    inline const sp<FreezeLock>&  getFreezeLock() const { return mFreezeLock; }
    // only for debugging
    inline PixelFormat pixelFormat() const { return mFormat; }

private:
    inline sp<Buffer> getFrontBuffer() {
        return mBuffers[mFrontBufferIndex];
    }
 
    void reloadTexture(const Region& dirty);

    sp<SurfaceBuffer> requestBuffer(int index, int usage);
    void destroy();

    class SurfaceLayer : public LayerBaseClient::Surface {
    public:
        SurfaceLayer(const sp<SurfaceFlinger>& flinger,
                SurfaceID id, const sp<Layer>& owner);
        ~SurfaceLayer();
    private:
        virtual sp<SurfaceBuffer> requestBuffer(int index, int usage);
        sp<Layer> getOwner() const {
            return static_cast<Layer*>(Surface::getOwner().get());
        }
    };
    friend class SurfaceLayer;
    
    sp<Surface>             mSurface;

            bool            mSecure;
            int32_t         mFrontBufferIndex;
            bool            mNeedsBlending;
            Region          mPostedDirtyRegion;
            sp<FreezeLock>  mFreezeLock;
            PixelFormat     mFormat;
            uint32_t        mBufferFlags;
            
            // protected by mLock
            sp<Buffer>      mBuffers[NUM_BUFFERS];
            Texture         mTextures[NUM_BUFFERS];
            uint32_t        mWidth;
            uint32_t        mHeight;
            
   mutable Mutex mLock;
};

// ---------------------------------------------------------------------------

}; // namespace android

#endif // ANDROID_LAYER_H
