import AbstractView from "../AbstractView.js";
import { formFromOptions } from "../form/Form.js";
import { getCalculatorOptionsByType, getCalculatorTypes } from "./Helpers.js";

export default class CalculatorAdd extends AbstractView {
    constructor(params) {
        super(params);
    }
    
    async getHtml() {
        const calculatorTypes = await getCalculatorTypes();
        const calculatorType = calculatorTypes[this.params.type];
        const calculatorOptions = await getCalculatorOptionsByType(this.params.type);

        this.setTitle(`Add ${calculatorType} calculator`);

        return `
            <h1>Add ${calculatorType} calculator</h1>
            ${formFromOptions("/api/calculators/add", calculatorOptions, "Add")}
        `;
    }
}
