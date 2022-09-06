// OpenGL super simple data view example for PrimeIT by Manuel Barão Rodrigues Soldado
//To show custom data please edit data.xml 
//I coded this after the primary interview, so it needs to be refactored to safely access class members and so on
//Just a proof of concept, I do intend to refactor it if interest is shown about this project
#include <fstream>
#include <iostream>
#include "pugixml.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <string>
#include <iostream>
#include <filesystem>
#include <random>






#include <cmath>

#include "fonts.h"





class PieChart : public sf::Drawable, public sf::Transformable
{
public:
	float total;
	sf::Text nameText;
	
	struct Slice
	{
		sf::Text text;
		std::string nameStr;
		float size;
		float scale;
		float explode;
		sf::Color color;
		Slice();
	};
	std::vector<Slice> slices;

	PieChart();
	void update();
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;
	void setRadius(float radius);
	void setDiameter(float diameter);

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	

private:
	sf::PrimitiveType m_primitive;
	std::vector<sf::Vertex> m_vertices;
	sf::Vector2f m_size;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

inline void PieChart::setSize(sf::Vector2f size)
{
	m_size = size;
}

inline sf::Vector2f PieChart::getSize() const
{
	return m_size;
}

inline void PieChart::setRadius(float radius)
{
	setDiameter(radius * 2.f);
}

inline void PieChart::setDiameter(float diameter)
{
	m_size = { diameter, diameter };
}

namespace
{

	inline float radiansFromDegrees(float degrees)
	{
		return degrees * 0.0174532925f; // pi/180
	}

} // namespace



	PieChart::Slice::Slice()
		: size{ 0.1f }
		, scale{ 1.f }
		, explode{ 0.f }
		, color(sf::Color::White)
	{
	}

	PieChart::PieChart()
		: slices()
		, m_primitive{ sf::PrimitiveType::Triangles }
		, m_vertices()
		, m_size({ 32.f, 32.f })
	{
	}

	void PieChart::update()
	{
		const sf::Vector2f halfSize{ m_size / 2.f };
		unsigned int totalNumberOfTrianglesRequired{ 0u };
		std::vector<unsigned int> numberOfTrianglesRequiredPerSlice;
		for (auto& slice : slices)
		{
			numberOfTrianglesRequiredPerSlice.emplace_back(static_cast<unsigned int>(std::floor(1.f + slice.size * 50.f)));
			totalNumberOfTrianglesRequired += numberOfTrianglesRequiredPerSlice.back();
		}
		m_vertices.resize(totalNumberOfTrianglesRequired * 3);
		unsigned int currentVertex{ 0u };
		float currentAngle{ 0.f };
		for (unsigned int slice{ 0u }; slice < slices.size(); ++slice)
		{
			const float startAngle{ currentAngle };
			const float halfAngleDifference{ 180.f * slices[slice].size };
			const sf::Vector2f offset{ sf::Vector2f(std::sin(radiansFromDegrees(startAngle + halfAngleDifference)), -std::cos(radiansFromDegrees(startAngle + halfAngleDifference))) * slices[slice].explode };
			for (unsigned int triangle{ 0u }; triangle < numberOfTrianglesRequiredPerSlice[slice]; ++triangle)
			{
				m_vertices[currentVertex + 0].position = halfSize + sf::Vector2f(offset.x * halfSize.x, offset.y * halfSize.y);
				m_vertices[currentVertex + 1].position = halfSize + sf::Vector2f((offset.x + std::sin(radiansFromDegrees(currentAngle))) * halfSize.x, (offset.y - std::cos(radiansFromDegrees(currentAngle))) * halfSize.y) * slices[slice].scale;
				currentAngle += halfAngleDifference * 2.f / numberOfTrianglesRequiredPerSlice[slice];
				m_vertices[currentVertex + 2].position = halfSize + sf::Vector2f((offset.x + std::sin(radiansFromDegrees(currentAngle))) * halfSize.x, (offset.y - std::cos(radiansFromDegrees(currentAngle))) * halfSize.y) * slices[slice].scale;
				m_vertices[currentVertex + 0].color = slices[slice].color;
				m_vertices[currentVertex + 1].color = slices[slice].color;
				m_vertices[currentVertex + 2].color = slices[slice].color;
				currentVertex += 3;
			}
		}
	}

	sf::FloatRect PieChart::getLocalBounds() const
	{
		sf::Vector2f topLeft{ m_vertices[0].position };
		sf::Vector2f bottomRight{ topLeft };
		for (auto& vertex : m_vertices)
		{
			if (vertex.position.x < topLeft.x)
				topLeft.x = vertex.position.x;
			else if (vertex.position.x > bottomRight.x)
				bottomRight.x = vertex.position.x;
			if (vertex.position.y < topLeft.y)
				topLeft.y = vertex.position.y;
			else if (vertex.position.y > bottomRight.y)
				bottomRight.y = vertex.position.y;
		}
		return{ topLeft, bottomRight - topLeft };
	}

	sf::FloatRect PieChart::getGlobalBounds() const
	{
		const sf::Transform transform{ getTransform() };
		sf::Vector2f topLeft{ transform.transformPoint(m_vertices[0].position) };
		sf::Vector2f bottomRight{ topLeft };
		sf::Vector2f current;
		for (auto& vertex : m_vertices)
		{
			current = transform.transformPoint(vertex.position);
			if (current.x < topLeft.x)
				topLeft.x = current.x;
			else if (current.x > bottomRight.x)
				bottomRight.x = current.x;
			if (current.y < topLeft.y)
				topLeft.y = current.y;
			else if (current.y > bottomRight.y)
				bottomRight.y = current.y;
		}
		return{ topLeft, bottomRight - topLeft };
	}



	// PRIVATE

	void PieChart::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();

		if (m_vertices.size() > 0)
			target.draw(&m_vertices.front(), m_vertices.size(), m_primitive, states);
	}




/*
class cPieChart {
public:
    float total {1};
    std::vector<cParcelas> parcelas;
    std::string name{ "" };
    float positionx{ 100 };
    float positiony{ 100 };
    float radius{ 100 };



};*/

	double round_up(double value, int decimal_places) {
		const double multiplier = std::pow(10.0, decimal_places);
		return std::ceil(value * multiplier) / multiplier;
	}

int main()
{

	sf::Font font;

	if (!font.loadFromMemory(_acfont, sizeof(_acfont)))
	{
		// error...
	}
    

    std::filesystem::current_path(std::filesystem::current_path()); // (3)
 
 

    std::string currentPath1 = std::filesystem::current_path().u8string();
    currentPath1 = currentPath1 + "\\data.xml";

    std::cout << currentPath1 << "\n";




    pugi::xml_document doc;
   

    pugi::xml_parse_result result = doc.load_file(currentPath1.c_str());

    
    //	pugi::xml_parse_result result = doc.load_buffer(bufferPainel, sizeof(bufferPainel));
    //std::cout << "Load result: " << result.description() << ",  name: " << doc.child("panel").attribute("name").as_string() << "\n";






    std::vector<PieChart> vPiecharts;


	std::string namePanel;
	pugi::xml_node panels = doc.child("panel");

	std::cout << panels.name() << std::endl;

	for (pugi::xml_node panel = panels.first_child(); panel; panel = panel.next_sibling())
	{
		std::string strDataShowType = "";
		float tempPosX = 150;
		float tempPosY = 150;
		float tempRadius = 100;
		std::string tempName = "";
		//We found a "Panel" -- print it's attributes
		for (pugi::xml_attribute attr = panel.first_attribute(); attr; attr = attr.next_attribute())
		{
			std::cout << " " << attr.name() << "=" << attr.value() << std::endl;
			std::string attrName = attr.name();
			if (!attrName.compare("name"))
			{
				namePanel = attr.value();
				tempName = namePanel;

			}
			else if (!attrName.compare("datashowtype"))
			{
				strDataShowType = attr.value();
				std::cout << "StrDatashowtype: " << strDataShowType << "\n";
			}else if (!attrName.compare("radius")) 
			{
				tempRadius = std::stof(attr.value());


			}
			else if (!attrName.compare("positionx")) {
				
				tempPosX = std::stof(attr.value());

			}
			else if (!attrName.compare("positiony")) {
				tempPosY = std::stof(attr.value());

			}
		}
		std::cout << std::endl;

		std::cout << "Agrupamento de dados: (interface) " << namePanel << std::endl;
		//Now print all elements and attributes of current "Panel"
		
		if (strDataShowType == "piechart")
		{
			PieChart oTempPieChart;
			float tempTotal = 0.0001; //Can't be acutally zero


			//For counting total
			for (pugi::xml_node child = panel.first_child(); child; child = child.next_sibling())
			{
				
				std::cout << "Nome do child: " << child.name() << std::endl;     // get element name
				// iterate through all attributes
				for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute())
				{
					std::cout << " Nome do atributo: " << attr.name() << "=" << attr.value() << std::endl;
					std::string attrName = attr.name();
					if (!attrName.compare("amount"))
					{
						std::string tempAmoutStr = attr.value();
						std::cout << "tempAmoutStr: " << tempAmoutStr << "\n";
						tempTotal = tempTotal + std::stof(tempAmoutStr);
					}
				}
				std::cout << std::endl;
			}

			for (pugi::xml_node child = panel.first_child(); child; child = child.next_sibling())
			{
				
				std::cout << child.name() << std::endl;     // get element name
				// iterate through all attributes
				int contadorteste = 0;
				

				for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute())
				{
					std::cout << " Novamente: " << attr.name() << "=" << attr.value() << std::endl;
					std::string attrName = attr.name();
					PieChart::Slice oTemparcela;
					
					if (!attrName.compare("name")) {
						std::string subtitlestr;
						subtitlestr = attr.value();
						
						oTemparcela.nameStr = subtitlestr;
						//std::cout << "aaaaa subtitlestr:" << oTemparcela.nameStr << "\n";
					}
					if (!attrName.compare("amount")) {


						
						std::string tempAmountStr = attr.value();
						oTemparcela.size = std::stof(tempAmountStr) / tempTotal;
						std::cout << "oTemparcela.size " << oTemparcela.size << "\n";
						std::random_device dev;
						std::mt19937 rng(dev());
						std::random_device dev2;
						std::mt19937 rng2(dev2());
						std::random_device dev3;
						std::mt19937 rng3(dev3());
						std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 255);
						
						//std::cout << randNum1 << "\n" << randNum2 << "\n" << randNum3 << "\n";
						oTemparcela.color = sf::Color(dist6(rng), dist6(rng2), dist6(rng3));



						oTemparcela.text.setFont(font); // font is a sf::Font
						oTemparcela.text.setFillColor(oTemparcela.color);
						
						oTemparcela.text.setPosition(tempPosX+100, tempPosY - 10*oTempPieChart.slices.size());
						oTemparcela.text.setCharacterSize(12); // in pixels, not points!
						std::string finalstr = child.attribute("name").as_string();	
						float normalizedPercentage = round_up(oTemparcela.size * 100,4);
						finalstr = finalstr + "   Value: " + std::to_string(normalizedPercentage) + "%";
						oTemparcela.text.setString(finalstr.c_str());
						//std::cout << "bbbbbb subtitlestr:" << oTemparcela.nameStr << "\n";
						
						


						oTempPieChart.slices.push_back(oTemparcela);
						

					}
					 

				}
				std::cout << std::endl;
			}
			oTempPieChart.nameText.setFont(font); // font is a sf::Font
			oTempPieChart.nameText.setFillColor(sf::Color(255, 255, 255));
			oTempPieChart.nameText.setString(tempName.c_str());
			oTempPieChart.nameText.setPosition(tempPosX-20, tempPosY + 100);
			oTempPieChart.nameText.setCharacterSize(24); // in pixels, not points!
			oTempPieChart.setPosition(tempPosX, tempPosY);
			oTempPieChart.setSize({ 150.f, 150.f });
			oTempPieChart.setOrigin(oTempPieChart.getSize() / 2.f);
			//std::cout << "Tempchart is: " << oTempPieChart. << "\n";
			vPiecharts.push_back(oTempPieChart);
		}
	}



	





    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Open GL data view works!");
    
	
	



    

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }



        window.clear();
		
		

		for (int contadorPieCharts = 0; contadorPieCharts < vPiecharts.size(); contadorPieCharts++)
		{
			//sf::CircleShape pieChartCircle(vPiecharts[contadorPieCharts].radius, 60);
			//pieChartCircle.setPosition(vPiecharts[contadorPieCharts].positionx, vPiecharts[contadorPieCharts].positiony);
			//pieChartCircle.setFillColor(sf::Color(100, 100, 100));
			//std::cout << "Parcelas do vetor: " << vPiecharts[contadorPieCharts].slices.size() << "\n";
			vPiecharts[contadorPieCharts].update();
			std::cout << "Slices vector size: " << vPiecharts[contadorPieCharts].slices.size() << "\n";
			/*std::cout << "Pos x: " << vPiecharts[contadorPieCharts].getPosition().x << "\n";
			std::cout << "Pos y: " << vPiecharts[contadorPieCharts].getPosition().y << "\n";
			std::cout << "Sizex: " << vPiecharts[contadorPieCharts].getSize().x << "\n";
			std::cout << "Sizey: " << vPiecharts[contadorPieCharts].getSize().y << "\n";*/
			window.draw(vPiecharts[contadorPieCharts]);
			window.draw(vPiecharts[contadorPieCharts].nameText);
			//std::cout << "Nametext: " << vPiecharts[contadorPieCharts].nameText << "\n";
			for (int contadorSlices = 0; contadorSlices <= vPiecharts[contadorPieCharts].slices.size()-1; contadorSlices++)
			{
				std::cout << "Slices Postion x: " << vPiecharts[contadorPieCharts].slices[contadorSlices].text.getPosition().x  << "\n";
				std::cout << "Slices Postion : " << vPiecharts[contadorPieCharts].slices[contadorSlices].text.getPosition().x << "\n";
				window.draw(vPiecharts[contadorPieCharts].slices[contadorSlices].text);
			}


		}

		
       
        window.display();
    }

    return 0;
}

