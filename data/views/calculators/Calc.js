import AbstractView from "../AbstractView.js";
import { getCalculatorConfig, getCalculatorDetails } from "./Helpers.js";

export default class Calculator extends AbstractView {
    constructor(params) {
        super(params);
    }

    async getHtml() {
        const config = await getCalculatorConfig(this.params.uuid);
        const details = await getCalculatorDetails(this.params.uuid);

        const keyValueDetails = Object.values(details).filter(item => item.type == "key/value");

        return `
            <h1>Calculator ${config.name} (${this.params.uuid})</h1>

            <table>
            ${keyValueDetails
                .map(item => `
                <tr>
                    <th>${item.label}</th>
                    <th>${item.value}</th>
                </tr>
                `)
                .join("\n")
            }
            </table>
        `;
    }
}
