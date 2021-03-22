
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.RectF.h"
#include "Effekseer.Vector2D.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RectF::RectF()
	: X(0.0f)
	, Y(0.0f)
	, Width(1.0f)
	, Height(1.0f)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RectF::RectF(float x, float y, float width, float height)
	: X(x)
	, Y(y)
	, Width(width)
	, Height(height)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Vector2D RectF::Position() const
{
	return Vector2D(X, Y);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Vector2D RectF::Size() const
{
	return Vector2D(Width, Height);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
  //----------------------------------------------------------------------------------
  //
  //----------------------------------------------------------------------------------