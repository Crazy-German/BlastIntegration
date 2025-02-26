// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2016-2024 NVIDIA Corporation. All rights reserved.

//! @file
//!
//! @brief Defines the API for the NvBlastExtTkFamily class

#ifndef NVBLASTTKFAMILY_H
#define NVBLASTTKFAMILY_H

#include "NvBlastTkIdentifiable.h"


// Forward declarations
struct NvBlastFamily;


namespace Nv
{
namespace Blast
{

// Forward declarations
class TkActor;
class TkAsset;
class TkEventListener;


/**
The TkFamily is associated with the TkActor that is instanced from a TkAsset, as well as all descendent TkActors generated
by spliting TkActors within the family.  It encapsulates an NvBlastFamily, and also holds a material which will be used
by default on all TkActors during damage functions.
*/
class TkFamily : public TkIdentifiable
{
public:
    /**
    Access to underlying low-level family.

    \return a pointer to the (const) low-level NvBlastFamily object.
    */
    virtual const NvBlastFamily*    getFamilyLL() const = 0;

    /**
    Every family has an associated asset (the TkAsset which was instanced to create the first member of the family).

    \return a pointer to the (const) TkAsset object.
    */
    virtual const TkAsset*          getAsset() const = 0;

    /**
    The number of actors currently in this family.

    \return the number of TkActors that currently exist in this family.
    */
    virtual uint32_t                getActorCount() const = 0;

    /**
    Retrieve an array of pointers (into the user-supplied buffer) to actors.

    \param[out] buffer      A user-supplied array of TkActor pointers.
    \param[in]  bufferSize  The number of elements available to write into buffer.
    \param[in]  indexStart  The starting index of the actor.

    \return the number of TkActor pointers written to the buffer.
    */
    virtual uint32_t                getActors(TkActor** buffer, uint32_t bufferSize, uint32_t indexStart = 0) const = 0;

    /**
    Add a user implementation of TkEventListener to this family's list of listeners.  These listeners will receive
    all split and fracture events generated by TkActor objects in this family.  They will also receive joint update events
    when TkJoint objects are updated that are (or were) associated with a TkActor in this family.

    \param[in]  l           The event listener to add.
    */
    virtual void                    addListener(TkEventListener& l) = 0;

    /**
    Remove a TkEventReciever from this family's list of listeners.

    \param[in]  l           The event listener to remove.
    */
    virtual void                    removeListener(TkEventListener& l) = 0;

    /**
    This function applies fracture buffers on relevant actors (actor which contains corresponding bond/chunk) in family.

    \param[in]  commands    The fracture commands to process.
    */
    virtual void                    applyFracture(const NvBlastFractureBuffers* commands) = 0;

    /**
    A function to reinitialize this family with new family. The Family must be created from the same low-level asset, but can be 
    in any other state.  As a result split events (TkEvent::Split) will be dispatched reflecting the resulting changes (created and removed actors)
    Afterwards the family will contain a copy of the new family and all actors' low-level actor pointers will be updated.

    \param[in] newFamily        The NvBlastFamily to use to reinitialize this family.
    \param[in] group            The group for new actors to be placed in.
    */
    virtual void                    reinitialize(const NvBlastFamily* newFamily, TkGroup* group = nullptr) = 0;
};

} // namespace Blast
} // namespace Nv


#endif // ifndef NVBLASTTKFAMILY_H
