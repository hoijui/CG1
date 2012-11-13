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
	Node* torso = new Node(
			0.0f, 0.0f, 0.0f, // position
			200.0f, 300.0f, 100.0f, // size
			0.0f, 0.0f, 0.0f, // joint position
			0.0f, 0.0f, 0.0f); // default rotation

	// XXX: implement more body/robot/whatever parts here

	// INSERT YOUR CODE HERE
	Node* head = new Node(
			0.0f, 165.0f, 0.0f, // position
			40.0f, 30.0f, 10.0f, // size
			0.0f, 0.0f, 0.0f, // joint position
			0.0f, 0.0f, 0.0f); // default rotation
	Node* leftLeg = new Node(
			60.0f, -400.0f, 0.0f, // position
			80.0f, 350.0f, 100.0f, // size
			0.0f, 0.0f, 0.0f, // joint position
			0.0f, 0.0f, 0.0f); // default rotation
	Node* rightLeg = new Node(
			-60.0f, -400.0f, 0.0f, // position
			80.0f, 350.0f, 100.0f, // size
			0.0f, 0.0f, 0.0f, // joint position
			0.0f, 0.0f, 0.0f); // default rotation

	// END XXX

	// finally, attach the robot/model parts
	// example: leftArm->setParent(torso), etc...
	// BUILD THE SCENEGRAPH

	// XXX: attach body parts here

	// INSERT YOUR CODE HERE
	head->setParent(torso);
	leftLeg->setParent(torso);
	rightLeg->setParent(torso);

	// END XXX

	// return root node
	return torso;
}
