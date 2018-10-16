module Main where

import System.Exit
import Test.HUnit
import Test.Framework
import Test.Framework.Providers.HUnit
import Hxsd.FFICalls
import Hxsd.Types
import Control.Exception(try, displayException, IOException, SomeException)

testBadRootDocFailsValidation = TestCase $ do
                                  (Right lxsd) <- parseSchemaFile "tests/vocabulary.xsd"
                                  (Right lxml) <- parseXmlString "<person_name xmlns=\"http://garbage.hxsd.org/api/terms/1.0\">\n    <frank/>\n</person_name>"
                                  (XmlFailsSchemaValidation errs) <- validateXmlAgainstSchema lxsd lxml
                                  assertBool "should have errors" (errs /= [])

testBadRootDocFailsSAXValidation = TestCase $ do
                                  (Right lxsd) <- parseSchemaFile "tests/vocabulary.xsd"
                                  (Right lxml) <- parseStreamingXmlString "<root></root>"
                                  (XmlFailsSchemaValidation errs) <- validateSAXAgainstSchema lxsd lxml
                                  assertBool "should have errors" (errs /= [])

testLoadXmlDocumentEmpty = TestCase $ do
                             lxml <- parseXmlString ""
                             assertBool "no file should be nothing" (lxml == (Left XmlParsingFailure))

testLoadXmlDocument = TestCase $ do
                         lxml <- parseXmlString "<root></root>"
                         assertBool "example file should parse" (lxml /= (Left XmlParsingFailure))

testLoadXmlDocumentSAXString = TestCase $ do
                         lxml <- parseStreamingXmlString "<root></root>"
                         assertBool "example file should parse" (lxml /= (Left XmlParsingFailure))

testLoadMissingSchema = TestCase $ do
                           lxml <- parseSchemaFile ""
                           assertBool "no file should be nothing" (lxml == (Left SchemaLoadFailure))

testIncludeSchema  = TestCase $ do
                                   lxml <- parseSchemaFile "tests/vocabulary.xsd"
                                   assertBool "schema should load correctly" (lxml /= (Left SchemaLoadFailure))

main = do 
        results <- runTestTT (
                               TestList [
                                 TestLabel "badRootFailsSAXValidation" testBadRootDocFailsSAXValidation,
                                 TestLabel "loadBlankXML" testLoadXmlDocumentEmpty,
                                 TestLabel "loadExampleFile" testLoadXmlDocument,
                                 TestLabel "loadSAXExampleString" testLoadXmlDocumentSAXString,
                                 TestLabel "loadIncludeSchema" testIncludeSchema,
                                 TestLabel "loadMissingSchema" testLoadMissingSchema,
                                 TestLabel "badRootFailsValidation" testBadRootDocFailsValidation ] )
        return ()