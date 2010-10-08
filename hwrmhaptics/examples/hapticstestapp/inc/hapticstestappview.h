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
* Description:  AppView class definition.
*
*/


#ifndef __HAPTICSTEST_APPVIEW_H__
#define __HAPTICSTEST_APPVIEW_H__

#include <coecntrl.h>

class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class CEikRichTextEditor;

/**
 * An instance of the Application View object for the hapticstest 
 * example application
 */
class CHapticsTestAppView : public CCoeControl
    {
public:

   /**
    * Create a ChapticstestAppView object, which will draw itself to aRect
    *
    * @param aRect the rectangle this view will be drawn to
    * @return a pointer to the created instance of ChapticstestAppView
    */
    static CHapticsTestAppView* NewL( const TRect& aRect );

   /**
    * Create a ChapticstestAppView object, which will draw itself to aRect
    *
    * @param aRect the rectangle this view will be drawn to
    * @return a pointer to the created instance of ChapticstestAppView
    */
    static CHapticsTestAppView* NewLC( const TRect& aRect );

   /**
    * Destroy the object and release all memory objects
    */
    ~CHapticsTestAppView();

   /**
    * Output the given line of text to the user.
    *
    * @param aOutputLine Text to be added to the end of the output.
    * @param aLineBreak Whether or not to add line break to the end
    *  of the print. By default the break is inserted.
    */
    void InsertOutput( const TDesC& aOutputLine, 
                       TBool aLineBreak = ETrue ) const;

   /**
    * Clears the output view.
    */
    void ClearOutput() const;

public:  // from CCoeControl

   /**
    * Draw this ChapticstestAppView to the screen
    *
    * @param aRect the rectangle of this view that needs updating
    */
    void Draw(const TRect& aRect) const;

   /**
    * @see CCoeControl
    */
    virtual void SizeChanged();

private:

   /**
    * Perform the second phase construction of a ChapticstestAppView object
    *
    * @param aRect the rectangle this view will be drawn to
    */
    void ConstructL(const TRect& aRect);

   /**
    * Perform the first phase of two phase construction 
    */
    CHapticsTestAppView();

private:

    CParaFormatLayer*   iParaFormat;
    CCharFormatLayer*   iCharFormat;
    CRichText*          iRichText;
    CEikRichTextEditor* iOutputWindow;
    };


#endif // __HAPTICSTEST_APPVIEW_H__
