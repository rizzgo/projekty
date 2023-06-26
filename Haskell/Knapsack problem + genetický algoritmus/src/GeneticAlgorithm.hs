-- Knapsack problem
-- Module - Genetic algorithm
-- Richard Seipel (xseipe00)
-- 2023

module GeneticAlgorithm (
    Parameters(..),
    solve
) where

import System.Random (mkStdGen, Random(randomRs, randoms), StdGen)
import Data.List (sortBy)
import Data.Ord (comparing)
import Types (Item(..), Knapsack(..))

type Chromosome = [Int]
type Fitness = Int
type GetGen = (Int -> [Int] -> StdGen)

data Parameters = Parameters {
    populationSize :: Int,
    crossoverProb :: Float,
    mutationProb :: Float,
    numOfIterations :: Int,
    numOfElites :: Int
}

solve :: StdGen -> Knapsack -> (Fitness, Chromosome)
solve gen knapsack =
    let randomNumbers = randoms gen
        getGen num nums = (mkStdGen $ nums!!num)
        numOfItems = length $ items knapsack
        parameters = Parameters {
            populationSize = 20*numOfItems,
            crossoverProb = 0.1,
            mutationProb = 0.05,
            numOfIterations = 15*numOfItems,
            numOfElites = 2
        }
        population = randomPopulation (getGen 0 randomNumbers) numOfItems (populationSize parameters)
        finalPopulation = runIterations getGen (tail randomNumbers) population knapsack parameters
        finalFitness = calculateFitness finalPopulation knapsack
    in  maximum $ zip finalFitness finalPopulation

randomPopulation :: StdGen -> Int -> Int -> [Chromosome]
randomPopulation gen numOfItems size =
    let randNumbers = take (numOfItems*size) (randomRs (0,1) gen) :: [Int]
        randChromosomes [] = []
        randChromosomes remaining = take numOfItems remaining : randChromosomes (drop numOfItems remaining)
    in randChromosomes randNumbers

runIterations :: GetGen -> [Int] -> [Chromosome] -> Knapsack -> Parameters -> [Chromosome]
runIterations _ _ population _ (Parameters _ _ _ 0 _) = population
runIterations getGen randomNumbers population knapsack parameters =
    runIterations getGen (drop 4 randomNumbers) newPopulation knapsack parameters {numOfIterations=numOfIterations parameters-1} 
    where
        fitness = calculateFitness population knapsack
        (elites, parents) = runTournament (getGen 0 randomNumbers) population fitness (numOfElites parameters)
        children = getSuccessors (getGen 1 randomNumbers) (getGen 2 randomNumbers) (crossoverProb parameters) parents
        mutated = mutate (getGen 3 randomNumbers) (mutationProb parameters) children
        newPopulation = elites++mutated

calculateFitness :: [Chromosome] -> Knapsack -> [Fitness]
calculateFitness population knapsack = map rateChromosome population
    where   
        weightSum chromosome = sum $ zipWith (*) (map weight $ items knapsack) chromosome
        costSum chromosome = sum $ zipWith (*) (map cost $ items knapsack) chromosome
        rateChromosome chromosome 
            | weightSum chromosome<=maxWeight knapsack = costSum chromosome 
            | otherwise = -1

runTournament :: StdGen -> [Chromosome] -> [Fitness] -> Int -> ([Chromosome], [Chromosome])
runTournament gen population fitness numberOfElites =
    let newPopSize = length population - numberOfElites
        pIndexes = take (newPopSize*2) (randomRs (0, length population-1) gen) :: [Int]
        elites = take numberOfElites . map snd $ sortFromTheFittest fitness population
    in (elites, selectParents pIndexes fitness population)

selectParents :: [Int] -> [Fitness] -> [Chromosome] -> [Chromosome]
selectParents (pIndex1:pIndex2:remaining) fitness population =
    if (fitness!!pIndex1) > (fitness!!pIndex2)
    then (population!!pIndex1):selectParents remaining fitness population
    else (population!!pIndex2):selectParents remaining fitness population
selectParents _ _ _ = []

getSuccessors :: StdGen -> StdGen -> Float -> [Chromosome] -> [Chromosome]
getSuccessors gen1 gen2 probOfCrossover parents =
    let crossoverPoints = take (length parents `div` 2) $ randomRs (1, length (head parents) - 1) gen1 :: [Int]
        probabilities = take (length parents `div` 2) $ randomRs (0,1) gen2 :: [Float]
    in crossover probOfCrossover parents crossoverPoints probabilities

crossover :: Float -> [Chromosome] -> [Int] -> [Float] -> [Chromosome]
crossover probOfCrossover (parent1:parent2:parents) (crossoverPoint:crossoverPoints) (prob:probabilities)
    | prob <= probOfCrossover = successor1:successor2:crossover probOfCrossover parents crossoverPoints probabilities
    | otherwise = parent1:parent2:crossover probOfCrossover parents crossoverPoints probabilities
    where
        successor1 = take crossoverPoint parent1 ++ drop crossoverPoint parent2
        successor2 = take crossoverPoint parent2 ++ drop crossoverPoint parent1
crossover _ [parent] _ _ = [parent]
crossover _ _ _ _ = []


mutate :: StdGen -> Float -> [Chromosome] -> [Chromosome]
mutate gen probOfMutation chromosomes =
    let probabilities = take (length chromosomes * chromosomeLength) (randomRs (0,1) gen) :: [Float]
    in processMutations chromosomeLength probOfMutation chromosomes probabilities
    where
        chromosomeLength = length $ head chromosomes
        
processMutations :: Int -> Float -> [Chromosome] -> [Float] -> [Chromosome]
processMutations chromosomeLength probOfMutation (chromosome:chromosomes) probabilities =
    zipWith (\chromBit prob -> if prob <= probOfMutation then 1-chromBit else chromBit)
    chromosome (take chromosomeLength probabilities) : processMutations chromosomeLength probOfMutation chromosomes (drop chromosomeLength probabilities)
processMutations _ _ _ _ = []


sortFromTheFittest :: [Int] -> [Chromosome] -> [(Fitness, Chromosome)]
sortFromTheFittest fitness population = reverse $ sortBy (comparing fst) (zip fitness population)
