class CornerPoint {
	public:
		  double PosX, PosY;
		  bool Valid;
		  LSFittedLine Line[2];
		  unsigned short CornerType, FieldObjectID;
		  short Orientation, Direction;
		  double visBearing;
		  double visDistance;
		  double visElevation;
};

