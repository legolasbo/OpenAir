const configurableCalculators = document.createElement("ul");

async function calculatorScreen() {
    const configurableCalculators = await getConfigurableCalculators();

    contentElement.innerText = "";
    contentElement.append(await listCalculators());
    contentElement.append(configurableCalculators);
}

async function editCalculatorScreen(uuid) {
    contentElement.innerText = uuid;
}

const createAddCalculatorForm = (calculatorType, options) => createElementWithAttributes(
        "form",
        {action: "/api/calculators/add", method: "post"},
        options.name,
        createHiddenInput("type", calculatorType),
        createSubmitButton("add")
    );

async function listCalculators() {
    const calculators = await fetchJson("/api/calculators");
    const tbody = createElement("tbody");
    const table = createElement("table",
        createElement("thead", 
            createElement("tr", 
                createElement("td", "Name"),
                createElement("td", "Type"),
                createElement("td", "Status"),
                createElement("td", "Actions"),
            )
        ),
        tbody,    
    );

    for (const [id, calculator] of Object.entries(calculators)) {
        const tr = createElement("tr",
            createElement("td", calculator.name),
            createElement("td", calculator.type),
            createElement("td", calculator.valid),
            createElement("td", 
                createButton("Edit", () => {editCalculatorScreen(id)}),
                createDeleteButton("/api/calculators/delete", id)
            ),
        );
        tbody.append(tr);
    }

    return table;
}

async function getConfigurableCalculators() {
    if (configurableCalculators.childNodes.length > 0) return configurableCalculators;

    const calculatorOptions = await fetchJson("/api/calculators/configurable");

    for (const [calculatorType, options] of Object.entries(calculatorOptions)) {
        const item = document.createElement("li");
        item.appendChild(createAddCalculatorForm(calculatorType, options));
        configurableCalculators.appendChild(item);
    }
    
    return configurableCalculators;
}
