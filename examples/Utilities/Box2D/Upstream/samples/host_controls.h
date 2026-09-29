// SPDX-FileCopyrightText: 2026 Raw Material Software Limited
// SPDX-License-Identifier: ISC

#pragma once

#include <stdarg.h>
#include <stdint.h>

#ifndef IM_ARRAYSIZE
#define IM_ARRAYSIZE( array ) ( (int) ( sizeof (array) / sizeof ( (array)[0] ) ) )
#endif

struct HostVec2
{
	float x = 0.0f;
	float y = 0.0f;
};

struct HostVec4
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};

struct HostDrawList
{
	void AddRectFilled( HostVec2, HostVec2, uint32_t ) {}
	void AddLine( HostVec2, HostVec2, uint32_t, float = 1.0f ) {}
	void AddCircleFilled( HostVec2, float, uint32_t ) {}
};

#define HOST_COL32( red, green, blue, alpha ) 	( (uint32_t)( ( (uint32_t)( alpha ) << 24 ) | ( (uint32_t)( blue ) << 16 ) | ( (uint32_t)( green ) << 8 ) | (uint32_t)( red ) ) )

namespace HostControls
{
bool beginPanel( const char* name );
void endPanel();
bool button( const char* label );
bool checkbox( const char* label, bool* value );
bool radioButton( const char* label, bool selected );
bool sliderFloat( const char* label, float* value, float minValue, float maxValue, const char* format = "%.3f" );
bool sliderFloat2( const char* label, float* values, float minValue, float maxValue, const char* format = "%.3f" );
bool sliderInt( const char* label, int* value, int minValue, int maxValue, const char* format = "%d" );
void textV( const char* format, va_list arguments );
void textDisabledV( const char* format, va_list arguments );
void textUnformatted( const char* text );
void textColoredV( HostVec4 colour, const char* format, va_list arguments );
void separator();
void spacing();
void dummy( HostVec2 size );
void pushItemWidth( float width );
void popItemWidth();
void pushStyleColor( int index, HostVec4 colour );
void popStyleColor( int count = 1 );
void sameLine( float offset = 0.0f, float spacing = 0.0f );
bool collapsingHeader( const char* label, int flags = 0 );
bool beginTabBar( const char* identifier, int flags = 0 );
void endTabBar();
bool beginTabItem( const char* label, bool* open = nullptr, int flags = 0 );
void endTabItem();
bool combo( const char* label, int* index, const char* const* items, int itemCount );
bool inputText( const char* label, char* buffer, int bufferLength );
void progressBar( float fraction, HostVec2 size, const char* overlay );
void openPopup( const char* identifier );
bool beginPopupModal( const char* name, bool* open, int flags );
void closeCurrentPopup();
bool selectable( const char* label, bool selected, int flags = 0 );
float getFontSize();
float getFrameHeight();
void setItemTooltipV( const char* format, va_list arguments );
bool begin( const char* name, bool* open, int flags );
void end();
bool beginTable( const char* identifier, int columns, int flags, float innerWidth, HostVec2 outerSize );
void endTable();
void tableNextRow( int rowFlags, float minRowHeight );
bool tableNextColumn();
void tableSetupColumn( const char* label, int flags, float initialWidthOrWeight );
void tableHeadersRow();
void setCursorPosX( float x );
HostVec2 getCursorScreenPos();
HostVec2 getItemRectMin();
HostVec2 getItemRectMax();
float getContentRegionAvailX();
HostVec2 getContentRegionAvail();
float getTextLineHeight();
HostDrawList* getWindowDrawList();
void setNextWindowPos( HostVec2 position, int condition, HostVec2 pivot = {} );
void setNextWindowSize( HostVec2 size, int condition );
void setNextWindowBgAlpha( float alpha );
bool treeNodeExV( const char* identifier, int flags, const char* label, va_list arguments );
void treePop();
bool menuItem( const char* label, const char* shortcut, bool* selected, bool enabled = true );
bool beginMenu( const char* label, bool enabled = true );
void endMenu();
bool beginMainMenuBar();
void endMainMenuBar();
bool beginChild( const char* identifier, HostVec2 size, int childFlags, int windowFlags = 0 );
void endChild();
bool beginListBox( const char* label, HostVec2 size = {} );
void endListBox();
void setScrollHereY( float centreYRatio = 0.5f );
bool isItemHovered( int flags = 0 );
uint32_t getColorU32( int index, float alphaMultiplier = 1.0f );
HostVec4 getStyleColorVec4( int index );
float getTextLineHeightWithSpacing();

inline bool Button( const char* label ) { return button( label ); }
inline bool Button( const char* label, HostVec2 ) { return button( label ); }
inline bool Checkbox( const char* label, bool* value ) { return checkbox( label, value ); }
inline bool RadioButton( const char* label, bool selected ) { return radioButton( label, selected ); }
inline bool RadioButton( const char* label, int* value, int selectedValue )
{
	if ( value != nullptr && radioButton( label, *value == selectedValue ) )
	{
		*value = selectedValue;
		return true;
	}

	return false;
}
inline bool SliderFloat( const char* label, float* value, float minValue, float maxValue, const char* format = "%.3f" )
{
	return sliderFloat( label, value, minValue, maxValue, format );
}
inline bool SliderFloat2( const char* label, float* values, float minValue, float maxValue, const char* format = "%.3f" )
{
	return sliderFloat2( label, values, minValue, maxValue, format );
}
inline bool SliderInt( const char* label, int* value, int minValue, int maxValue, const char* format = "%d" )
{
	return sliderInt( label, value, minValue, maxValue, format );
}
inline void Text( const char* format, ... )
{
	va_list arguments;
	va_start( arguments, format );
	textV( format, arguments );
	va_end( arguments );
}
inline void TextDisabled( const char* format, ... )
{
	va_list arguments;
	va_start( arguments, format );
	textDisabledV( format, arguments );
	va_end( arguments );
}
inline void TextUnformatted( const char* text ) { textUnformatted( text ); }
inline void TextColored( HostVec4 colour, const char* format, ... )
{
	va_list arguments;
	va_start( arguments, format );
	textColoredV( colour, format, arguments );
	va_end( arguments );
}
inline void Separator() { separator(); }
inline void Spacing() { spacing(); }
inline void Dummy( HostVec2 size ) { dummy( size ); }
inline void PushItemWidth( float width ) { pushItemWidth( width ); }
inline void PopItemWidth() { popItemWidth(); }
inline void SameLine( float offset = 0.0f, float spacing = 0.0f ) { sameLine( offset, spacing ); }
inline bool CollapsingHeader( const char* label, int flags = 0 ) { return collapsingHeader( label, flags ); }
inline bool BeginTabBar( const char* id, int flags = 0 ) { return beginTabBar( id, flags ); }
inline void EndTabBar() { endTabBar(); }
inline bool BeginTabItem( const char* label, bool* open = nullptr, int flags = 0 ) { return beginTabItem( label, open, flags ); }
inline void EndTabItem() { endTabItem(); }
inline bool Combo( const char* label, int* index, const char* const* items, int itemCount )
{
	return combo( label, index, items, itemCount );
}
inline bool InputText( const char* label, char* buffer, int bufferLength ) { return inputText( label, buffer, bufferLength ); }
inline void ProgressBar( float fraction, HostVec2 size, const char* overlay ) { progressBar( fraction, size, overlay ); }
inline void OpenPopup( const char* id ) { openPopup( id ); }
inline bool BeginPopupModal( const char* name, bool* open = nullptr, int flags = 0 ) { return beginPopupModal( name, open, flags ); }
inline void CloseCurrentPopup() { closeCurrentPopup(); }
inline bool Selectable( const char* label, bool selected, int flags = 0 ) { return selectable( label, selected, flags ); }
inline float GetFontSize() { return getFontSize(); }
inline float GetFrameHeight() { return getFrameHeight(); }
inline void SetItemTooltip( const char* format, ... )
{
	va_list arguments;
	va_start( arguments, format );
	setItemTooltipV( format, arguments );
	va_end( arguments );
}
inline bool Begin( const char* name, bool* open = nullptr, int flags = 0 ) { return begin( name, open, flags ); }
inline void End() { end(); }
inline bool BeginTable( const char* id, int columns, int flags = 0, HostVec2 outerSize = {}, float innerWidth = 0.0f )
{
	return beginTable( id, columns, flags, innerWidth, outerSize );
}
inline void EndTable() { endTable(); }
inline void TableNextRow( int rowFlags = 0, float minRowHeight = 0.0f ) { tableNextRow( rowFlags, minRowHeight ); }
inline bool TableNextColumn() { return tableNextColumn(); }
inline void TableSetupColumn( const char* label, int flags = 0, float initWidthOrWeight = 0.0f )
{
	tableSetupColumn( label, flags, initWidthOrWeight );
}
inline void TableHeadersRow() { tableHeadersRow(); }
inline void SetCursorPosX( float x ) { setCursorPosX( x ); }
inline HostVec2 GetCursorScreenPos() { return getCursorScreenPos(); }
inline HostVec2 GetItemRectMin() { return getItemRectMin(); }
inline HostVec2 GetItemRectMax() { return getItemRectMax(); }
inline float GetContentRegionAvailX() { return getContentRegionAvailX(); }
inline HostVec2 GetContentRegionAvail() { return getContentRegionAvail(); }
inline float GetTextLineHeight() { return getTextLineHeight(); }
inline HostDrawList* GetWindowDrawList() { return getWindowDrawList(); }
inline void SetNextWindowPos( HostVec2 pos, int condition, HostVec2 pivot = {} ) { setNextWindowPos( pos, condition, pivot ); }
inline void SetNextWindowSize( HostVec2 size, int condition ) { setNextWindowSize( size, condition ); }
inline void SetNextWindowBgAlpha( float alpha ) { setNextWindowBgAlpha( alpha ); }
inline bool TreeNodeEx( const char* identifier, int flags, const char* label, ... )
{
	va_list arguments;
	va_start( arguments, label );
	bool isOpen = treeNodeExV( identifier, flags, label, arguments );
	va_end( arguments );
	return isOpen;
}
inline void TreePop() { treePop(); }
inline bool MenuItem( const char* label, const char* shortcut, bool* selected, bool enabled = true )
{
	return menuItem( label, shortcut, selected, enabled );
}
inline bool BeginMenu( const char* label, bool enabled = true ) { return beginMenu( label, enabled ); }
inline void EndMenu() { endMenu(); }
inline bool BeginMainMenuBar() { return beginMainMenuBar(); }
inline void EndMainMenuBar() { endMainMenuBar(); }
inline bool BeginChild( const char* id, HostVec2 size, int childFlags, int windowFlags = 0 )
{
	return beginChild( id, size, childFlags, windowFlags );
}
inline void EndChild() { endChild(); }
inline bool BeginListBox( const char* label, HostVec2 size = {} ) { return beginListBox( label, size ); }
inline void EndListBox() { endListBox(); }
inline void SetScrollHereY( float centerYRatio = 0.5f ) { setScrollHereY( centerYRatio ); }
inline bool IsItemHovered( int flags = 0 ) { return isItemHovered( flags ); }
inline uint32_t GetColorU32( int index, float alphaMul = 1.0f ) { return getColorU32( index, alphaMul ); }
inline HostVec4 GetStyleColorVec4( int index ) { return getStyleColorVec4( index ); }
inline float GetTextLineHeightWithSpacing() { return getTextLineHeightWithSpacing(); }
} // namespace HostControls
