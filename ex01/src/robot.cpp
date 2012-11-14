/* ----------------------------------------------------------------
name:           robot.c
purpose:        implementation of the scenegraph construction
version:	   SKELETON CODE
TODO:           buildRobot
author:         katrin lang
computer graphics
tu berlin
------------------------------------------------------------- */

#include "robot.h"

// nodes of the scenegraph

// XXX: CURRENTLY ONLY TORSO. INSERT OTHER BODY PARTS HERE.
//      SEE EXAMPLE BINARY cg1_ex1.exe (win32) and cg1_ex1 (linux).
//      EXPERIMENTATION IS ENCOURAGED. THE FINAL MODEL/SCENEGRAPH
//      NEED NOT BE IDENTICAL TO THE PROPOSED SOLUTION.

// XXX: EXTEND THE SCENEGRAPH i.e.
// Node *part1, *part2, *part3, etc...

// build the robot scenegraph
Node *buildRobot(){

	// torso attributes and initial position.
	// for parameters of constructor and their meanings,
	// see file node.h
	Node *torso0= new Node(0, 0, 0,
			150, 150, 200,
			0, 0, 0,
			0,0,0);//90, 180, 0);

	// XXX: implement more body/robot/whatever parts here

	// INSERT YOUR CODE HERE

	Node *torso1= new Node(0, 0, -200,
			140, 120, 140,
			0, 0, 85,
			0, 0, 0);

	Node *head0= new Node(0, 0, 170,
			100, 100, 100,
			0, 0, -62,
			0, 0, 0);
	Node *head1= new Node(0, 0, 50,
			80, 80, 80,
			0, 0, 0,
			0, 0, 0);
	Node *leftEye= new Node(18, 50, 38,
			15, 15, 15,
			0,0,0,
			0, 0, 0);
	Node *rightEye= new Node(-18, 50, 38,
			15, 15, 15,
			0,0,0,
			0, 0, 0);

	Node *leftLeg0= new Node(40, 0, -209,
			70.8, 70.8, 216,
			0,0,120,
			0, 0, 0);
	Node *leftLeg1= new Node(0, 0, -239,
			70.8, 70.8, 216,
			0,0,120,
			0, 0, 0);
	Node *leftLeg2= new Node(0, 27, -144,
			59, 177, 23,
			0,-27,28,
			0, 0, 0);

	Node *rightLeg0= new Node(-40, 0, -209,
			70.8, 70.8, 216,
			0,0,120,
			0, 0, 0);
	Node *rightLeg1= new Node( 0, 0, -239,
			70.8, 70.8, 216,
			0,0,120,
			0, 0, 0);
	Node *rightLeg2= new Node( 0, 27, -144,
			59, 177, 23,
			0,-27,28,
			0, 0, 0);

	Node *leftArm0= new Node(110, 0, -34,
			55, 55, 157,
			0, 0, 84,
			0, 0, 0);
	Node *leftArm1= new Node(0, 0, -168,
			55, 55, 157,
			0, 0, 84,
			0, 0, 0);
	Node *leftArm2= new Node(0, 0, -135,
			47, 23, 98,
			0, 0, 53,
			0, 0, 0);

	Node *rightArm0= new Node(-110, 0, -34,
			55, 55, 157,
			0, 0, 84,
			0, 0, 0);
	Node *rightArm1= new Node(0, 0, -168,
			55, 55, 157,
			0, 0, 84,
			0, 0, 0);
	Node *rightArm2= new Node( 0, 0, -135,
			47, 23, 98,
			0, 0, 53,
			0, 0, 0);

	// END XXX

	// finally, attach the robot/model parts
	// example: leftArm->setParent(torso), etc...
	// BUILD THE SCENEGRAPH

	// XXX: attach body parts here

	// INSERT YOUR CODE HERE
	//head->setParent(torso);
	//leftLeg->setParent(torso);
	//rightLeg->setParent(torso);

	torso1->setParent(torso0);

	head0->setParent(torso0);
	head1->setParent(head0);
	leftEye->setParent(head0);
	rightEye->setParent(head0);

	leftLeg0->setParent(torso1);
	leftLeg1->setParent(leftLeg0);
	leftLeg2->setParent(leftLeg1);

	rightLeg0->setParent(torso1);
	rightLeg1->setParent(rightLeg0);
	rightLeg2->setParent(rightLeg1);

	leftArm0->setParent(torso0);
	leftArm1->setParent(leftArm0);
	leftArm2->setParent(leftArm1);

	rightArm0->setParent(torso0);
	rightArm1->setParent(rightArm0);
	rightArm2->setParent(rightArm1);

	// END XXX

	// return root node
	return torso0;
}
