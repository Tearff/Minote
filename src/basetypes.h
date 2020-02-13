/**
 * Semantic structures for dealing with coordinates, sizes and colors
 * @file
 * Alignment is ensured so that the x/y/z members and array access can be used
 * interchangeably.
 */

#ifndef MINOTE_BASETYPES_H
#define MINOTE_BASETYPES_H

/// An integer position in 2D space
typedef union point2i {
	struct {
		int x;
		int y;
	};
	int arr[2]; ///< array representation
} point2i;

/// An integer 2D size. Members should not be negative.
typedef point2i size2i;

/// An integer position in 3D space
typedef union point3i {
	struct {
		int x;
		int y;
		int z;
	};
	int arr[3]; ///< array representation
} point3i;

/// An integer 3D size. Members should not be negative.
typedef point3i size3i;

/// A floating-point position in 2D space
typedef union point2f {
	struct {
		float x;
		float y;
	};
	float arr[2]; ///< array representation
} point2f;

/// A floating-point 2D size. Members should not be negative.
typedef point2f size2f;

/// A floating-point position in 3D space
typedef union point3f {
	struct {
		float x;
		float y;
		float z;
	};
	float arr[3]; ///< array representation
} point3f;

/// A floating-point 3D size. Members should not be negative.
typedef point3f size3f;

/// An RGB color triple. Values higher than 1.0 represent HDR.
typedef union color3 {
	struct {
		float r;
		float g;
		float b;
	};
	float arr[3]; ///< array representation
} color3;

/// An RGBA color quad. Values higher than 1.0 represent HDR.
typedef union color4 {
	struct {
		float r;
		float g;
		float b;
		float a;
	};
	float arr[4]; ///< array representation
} color4;

/// White color convenience constant
#define Color4White ((color4){1.0f, 1.0f, 1.0f, 1.0f})

/// Fully transparent color convenience constant
#define Color4Clear ((color4){1.0f, 1.0f, 1.0f, 0.0f})

/**
 * Copy the value of a color3 into another.
 * @param dst Destination color3
 * @param src Source color3
 */
#define color3Copy(dst, src) \
    arrayCopy((dst).arr, (src).arr)

/**
 * Copy the value of a color4 into another.
 * @param dst Destination color4
 * @param src Source color4
 */
#define color4Copy(dst, src) \
    arrayCopy((dst).arr, (src).arr)

/**
 * Convert a ::color3 from sRGB to Linear color space.
 * @param color Input color
 * @return Output color
 */
color3 color3ToLinear(color3 color);

#endif //MINOTE_BASETYPES_H