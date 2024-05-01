const calculatorCache = {
    types: null,
}

export const getCalculatorTypes = async () => {
    if (!calculatorCache.types) {
        calculatorCache.types = await fetch("/api/calculators/types")
        .then(response => response.json());
    }
    return calculatorCache.types;
};

export const clearCalculatorCache = () => {
    calculatorCache.types = null;
}

export const getCalculatorConfig = async (uuid) => await fetch(`/api/calculators/get?uuid=${uuid}`)
    .then(response => response.json());
    
export const getCalculatorOptionsByType = async (type) => await fetch(`/api/calculators/options?type=${type}`)
    .then(response => response.json());
export const getCalculatorOptionsByUuid = async (uuid) => await fetch(`/api/calculators/options?uuid=${uuid}`)
    .then(response => response.json());

export const getCalculatorDetails = async (uuid) => await fetch(`/api/calculators/details?uuid=${uuid}`)
    .then(response => response.json());
