import AbstractView from "../AbstractView.js";
import { formFromOptions } from "../form/Form.js";
import { getCalculatorConfig, getCalculatorOptionsByUuid, getCalculatorTypes } from "./Helpers.js";

export default class CalculatorEdit extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle(`Edit calculator: ${this.params.uuid}`);
    }

    async getHtml() {
        const [options, config, calculatorTypes] = await Promise.all([
            getCalculatorOptionsByUuid(this.params.uuid),
            getCalculatorConfig(this.params.uuid),
            getCalculatorTypes(),
        ]);
        const calculatorType = calculatorTypes[config.type];

        const assignIfKeyExists = ([k, v]) => {
            if (options[k] != undefined) {
                options[k].value = v;
            }
        }

        Object.entries(config).forEach(assignIfKeyExists);
        if (config.options) {
            Object.entries(config.options).forEach(assignIfKeyExists);
        }
        delete(options.type);
        
        this.setTitle(`Edit ${calculatorType} calculator`);

        return `
            <h1>Edit ${calculatorType} calculator</h1>
            ${formFromOptions("/api/calculators/edit", options, "Edit")}
            <a href="/calculators" data-link="">Cancel</a>
        `;
    }
}
