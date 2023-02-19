#line 2 "TestQueue.ino"

#include <AUnitVerbose.h>
#include <Parameter.h>
#include <Queue.h>
#include <Constants.h>

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
    Queue q;
  }
  assertTrue(true);
}

test(QueueTest, test_empty_withItems) {
  Queue q;
  q.push(&param1);
  assertFalse(q.empty());
}

test(QueueTest, test_empty_withoutItems) {
  Queue q;
  assertTrue(q.empty());
}

test(QueueTest, test_pop_) {
  Queue q;
  q.push(&param1);
  assertEqual(&param1, q.pop());
}

test(QueueTest, test_pop_paramBool) {
  Queue q;
  q.push(&paramBool);
  assertEqual(&paramBool, q.pop());
}

test(QueueTest, test_pop_paramInt) {
  Queue q;
  q.push(&paramInt);
  assertEqual(&paramInt, q.pop());
}

test(QueueTest, test_pop_paramDouble) {
  Queue q;
  q.push(&paramDouble);
  assertEqual(&paramDouble, q.pop());
}

test(QueueTest, test_pop_withoutItems) {
  Queue q;
  assertEqual(NULL, q.pop());
}

test(QueueTest, test_pop_firstItem) {
  Queue q;
  Parameter param2(1);
  q.push(&param1);
  q.push(&param2);
  assertEqual(&param1, q.pop());
}

test(QueueTest, test_pop_itemDeleted) {
  Queue q;
  Parameter param2(1);
  q.push(&param1);
  q.push(&param2);
  assertEqual(&param1, q.pop());
  assertEqual(&param2, q.pop());
  assertEqual(NULL, q.pop());
}

test(QueueTest, test_push_) {
  Queue q;
  assertTrue(q.push(&param1));
}

test(QueueTest, test_push_paramBool) {
  Queue q;
  assertTrue(q.push(&paramBool));
}

test(QueueTest, test_push_paramInt) {
  Queue q;
  assertTrue(q.push(&paramInt));
}

test(QueueTest, test_push_paramDouble) {
  Queue q;
  assertTrue(q.push(&paramDouble));
}

test(QueueTest, test_push_queueFull) {
  Queue q;
  // Fill queue
  for (int i=0; i<QUEUE_SIZE; i++) {
    assertTrue(q.push(&param1));
  }
  // Try putting one additional element
  assertFalse(q.push(&param1));
}
