-- Knapsack problem
-- Main file
-- Richard Seipel (xseipe00)
-- 2023

import System.Environment (getArgs)
import System.Random (StdGen, getStdGen)
import Parser (parse) 
import Types (Knapsack(..), Solution)
import BruteForce (solve)
import GeneticAlgorithm (solve)

main :: IO ()
main = do
    args <- getArgs
    let (mode, nextArgs) = getMode args
    gen <- getStdGen
    contents <- case nextArgs of
        Just (filePath:_) -> readFile filePath
        Just [] -> getContents
        Nothing -> return ""
    let knapsack = parse contents
    putStrLn $ getSolution gen mode knapsack

info :: Knapsack -> String
info = show

bruteForce :: Knapsack -> String
bruteForce knapsack =
    formatOutput . checkMinCost (minCost knapsack) $ BruteForce.solve knapsack

optimMethod :: StdGen -> Knapsack -> String
optimMethod gen knapsack =
    formatOutput . checkMinCost (minCost knapsack) $ GeneticAlgorithm.solve gen knapsack

getMode :: [String] -> (String, Maybe [String])
getMode args = case args of
    (mode:nextArgs) | mode `elem` ["-i", "-b", "-o"] -> (mode, Just nextArgs)
    [] -> ("", Just [])
    _ -> ("", Nothing)

getSolution :: StdGen -> String -> Maybe Knapsack -> String
getSolution gen mode input = case input of
    (Just knapsack) -> if items knapsack /= []
        then chooseAction gen mode knapsack
        else "Error: No items."
    Nothing -> "Error: Parsing error"

chooseAction :: StdGen -> String -> Knapsack -> String
chooseAction gen mode knapsack = case mode of
    "-i" -> info knapsack
    "-b" -> bruteForce knapsack
    "-o" -> optimMethod gen knapsack
    arg -> "Error: Invalid argument " ++ arg ++ "."

formatOutput :: Maybe Solution -> String
formatOutput output = case output of
    Just solution -> "Solution [" ++ unwords (map show solution) ++ "]"
    Nothing -> "False"

checkMinCost :: Int -> (Int, Solution) -> Maybe Solution
checkMinCost minimalCost (cost, result) = if minimalCost<=cost then Just result else Nothing
