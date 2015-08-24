module Main where

import Test.HUnit
import Test.Framework
import Test.Framework.Providers.HUnit

import Hxsd.FFICalls

testLoadXmlDocumentEmpty = TestCase $ do
                         lxml <- parseXmlString ""
                         assertBool "no file should be nothing" (lxml == Nothing)

testLoadXmlDocument = TestCase $ do
                         lxml <- parseXmlString "<root></root>"
                         assertBool "example file should not be nothing" (lxml /= Nothing)


main = defaultMain $ hUnitTestToTests $
          TestList [TestLabel "loadBlankXML" testLoadXmlDocumentEmpty,
                   TestLabel "loadExampleFile" testLoadXmlDocument]
