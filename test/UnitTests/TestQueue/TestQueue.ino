#line 2 "TestQueue.ino"

#include <AUnitVerbose.h>
#include "Parameter.h"
#include "Queue.h"
#include "Constants.h"

using aunit::TestRunner;


Parameter param1(0);
ParameterBool paramBool(0);
ParameterInt paramInt(0);
ParameterDouble paramDouble(0);


// Prepare the serial port, include/exclude tests
void setup() {
    Serial.begin(9600);
    delay(50);
    TestRunner::setTimeout(120);
    //TestRunner::exclude("*");
    //TestRunner::include("*");
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

test(QueueTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    Queue q(sizeof(&param1));
  }
  assertTrue(true);
}

test(QueueTest, test_constructor_paramDouble) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    Queue q(sizeof(&paramDouble));
  }
  assertTrue(true);
}

test(QueueTest, test_constructor_paramInt) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    Queue q(sizeof(&paramInt));
  }
  assertTrue(true);
}

test(QueueTest, test_constructor_paramBool) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    Queue q(sizeof(&paramBool));
  }
  assertTrue(true);
}

test(QueueTest, test_empty_withItems) {
  Queue q(sizeof(&param1));
  q.push(&param1);
  assertFalse(q.empty());
}

test(QueueTest, test_empty_withoutItems) {
  Queue q(sizeof(&param1));
  assertTrue(q.empty());
}

test(QueueTest, test_pop_) {
  Queue q(sizeof(&param1));
  q.push(&param1);
  assertEqual(&param1, q.pop());
}

test(QueueTest, test_pop_paramBool) {
  Queue q(sizeof(&param1));
  q.push(&paramBool);
  assertEqual(&paramBool, q.pop());
}

test(QueueTest, test_pop_paramInt) {
  Queue q(sizeof(&param1));
  q.push(&paramInt);
  assertEqual(&paramInt, q.pop());
}

test(QueueTest, test_pop_paramDouble) {
  Queue q(sizeof(&param1));
  q.push(&paramDouble);
  assertEqual(&paramDouble, q.pop());
}

test(QueueTest, test_pop_withoutItems) {
  Queue q(sizeof(&param1));
  assertEqual(NULL, q.pop());
}

test(QueueTest, test_pop_firstItem) {
  Queue q(sizeof(&param1));
  Parameter param2(1);
  q.push(&param1);
  q.push(&param2);
  assertEqual(&param1, q.pop());
}

test(QueueTest, test_pop_itemDeleted) {
  Queue q(sizeof(&param1));
  Parameter param2(1);
  q.push(&param1);
  q.push(&param2);
  assertEqual(&param1, q.pop());
  assertEqual(&param2, q.pop());
  assertEqual(NULL, q.pop());
}

test(QueueTest, test_push_) {
  Queue q(sizeof(param1));
  assertTrue(q.push(&param1));
}

test(QueueTest, test_push_paramBool) {
  Queue q(sizeof(param1));
  assertTrue(q.push(&paramBool));
}

test(QueueTest, test_push_paramInt) {
  Queue q(sizeof(param1));
  assertTrue(q.push(&paramInt));
}

test(QueueTest, test_push_paramDouble) {
  Queue q(sizeof(param1));
  assertTrue(q.push(&paramDouble));
}

test(QueueTest, test_push_queueFull) {
  Queue q(sizeof(param1));
  // Fill queue
  for (int i=0; i<QUEUE_SIZE; i++) {
    assertTrue(q.push(&param1));
  }
  // Try putting one additional element
  assertFalse(q.push(&param1));
}
