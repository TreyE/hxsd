module Hxsd.Types where

data XmlParsingFailure = XmlParsingFailure deriving Eq
data SchemaLoadFailure = SchemaLoadFailure deriving Eq

type XmlParsingResult a = Either XmlParsingFailure a
type SchemaLoadResult a = Either SchemaLoadFailure a
