/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  AppView class implementation.
*
*/


#include <coemain.h>
#include <eikenv.h>
#include <eikrted.h>
#include <txtrich.h>
#include <hapticstest.rsg>

#include "hapticstestappview.h"

const TInt KTestSmallerFontTwips = 65;

// ---------------------------------------------------------
// Standard construction sequence
// ---------------------------------------------------------
//
CHapticsTestAppView* CHapticsTestAppView::NewL(const TRect& aRect)
    {
    CHapticsTestAppView* self = CHapticsTestAppView::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// Static constructor. Leaves the created instance onto the
// cleanup stack.
// ---------------------------------------------------------
//
CHapticsTestAppView* CHapticsTestAppView::NewLC(const TRect& aRect)
    {
    CHapticsTestAppView* self = new (ELeave) CHapticsTestAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
//
CHapticsTestAppView::CHapticsTestAppView()
    {
    // no implementation required
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
CHapticsTestAppView::~CHapticsTestAppView()
    {
    delete iParaFormat;
    iParaFormat = NULL;
    
    delete iCharFormat;
    iCharFormat = NULL;
    
    delete iOutputWindow;
    iOutputWindow = NULL;
    
    delete iRichText;
    iRichText = NULL;
    }

// ---------------------------------------------------------
// Construct the view using a rich text editor.
// ---------------------------------------------------------
//
void CHapticsTestAppView::ConstructL(const TRect& aRect)
    {
    // Create a window for this application view
    CreateWindowL();
    
    // get normal font
    const CFont* font = CEikonEnv::Static()->NormalFont();
    TFontSpec fontspec = font->FontSpecInTwips();

    TCharFormat defaultCharFormat( fontspec.iTypeface.iName, 
                                   fontspec.iHeight - KTestSmallerFontTwips );
    TCharFormatMask defaultCharFormatMask;
    defaultCharFormatMask.SetAttrib( EAttFontTypeface );
    defaultCharFormatMask.SetAttrib( EAttFontHeight );

    // paragraph and char format layers
    iParaFormat = CParaFormatLayer::NewL();
    iCharFormat = CCharFormatLayer::NewL( defaultCharFormat,
                                          defaultCharFormatMask );
    
    iRichText = CRichText::NewL( iParaFormat, iCharFormat );
    
    //Create bordered edit windows.
    iOutputWindow = new (ELeave) CEikRichTextEditor( TGulBorder( TGulBorder::EShallowRaised ) );
    iOutputWindow->SetContainerWindowL( *this );

    // Create scrollbar
    iOutputWindow->CreateScrollBarFrameL();
    iOutputWindow->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    
    iOutputWindow->ConstructL( this, 0, 0, 
        EEikEdwinOwnsWindow|EEikEdwinInclusiveSizeFixed|
        EEikEdwinKeepDocument|EEikEdwinUserSuppliedText|
        EEikEdwinLineCursor|EEikEdwinAlwaysShowSelection );
    
    TRect outputRect( Rect() );
    TInt outputWidthOffset = iOutputWindow->ScrollBarFrame()->ScrollBarBreadth( CEikScrollBar::EVertical );
    outputRect.iBr.iX -= outputWidthOffset;
    
    iOutputWindow->SetDocumentContentL( *iRichText, CEikEdwin::EUseText );
    iOutputWindow->SetRect( outputRect );
    iOutputWindow->ActivateL();
    iOutputWindow->SetFocus( ETrue );

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// ---------------------------------------------------------
// Insert the given text to the end of the outputted text.
// Adds line break to the end of the text.
// ---------------------------------------------------------
//
void CHapticsTestAppView::InsertOutput( const TDesC& aOutputLine,
                                        TBool aLineBreak ) const
    {
    TRAPD( err, iRichText->InsertL( iOutputWindow->TextLength(), 
                                    aOutputLine ) );

    // insert line break if requested
    if ( aLineBreak )
        {
        TRAP( err, iRichText->InsertL( iOutputWindow->TextLength(), 
                                      (TChar)CEditableText::ELineBreak ) );
        }
    
    TRAP( err, iOutputWindow->ForceScrollBarUpdateL() );
    TRAP( err, iOutputWindow->SetCursorPosL( iOutputWindow->TextLength(),
                                             EFalse) );
    }

// ---------------------------------------------------------
// Clears the output view.
// ---------------------------------------------------------
//
void CHapticsTestAppView::ClearOutput() const
    {
    iRichText->Reset();
    TRAPD( err, iOutputWindow->HandleTextChangedL());
    err = err; // just to escape warning message for ARM
    iOutputWindow->SetCursorPosL( 0, EFalse );
    }

// ---------------------------------------------------------
// Draw this application's view to the screen
// ---------------------------------------------------------
//
void CHapticsTestAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context 
    CWindowGc& gc = SystemGc();
    
    // Gets the control's extent
    TRect rect = Rect();
    
    // Clears the screen
    gc.Clear( rect );
    }

// ---------------------------------------------------------
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CHapticsTestAppView::SizeChanged()
    {
    // decrease the width of the scrollbar from the width of the editor rect
    TInt outputWidthOffset = iOutputWindow->ScrollBarFrame()->ScrollBarBreadth( CEikScrollBar::EVertical );
    TRect outputRect( Rect() );
    outputRect.iBr.iX -= outputWidthOffset;
    iOutputWindow->SetRect( outputRect );
    iOutputWindow->SetFocus( ETrue );
    }


